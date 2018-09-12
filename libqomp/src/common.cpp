/*
 * Copyright (C) 2013-2017  Khryukin Evgeny
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "common.h"
#include "defines.h"

#include <QTime>
#include <QTextDocument>
#include <QStandardPaths>
#include <QDir>
#include <QTextCodec>
#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#else
#include "ganalytics.h"
#include <QWidget>
#include <QLayout>
#include <QApplication>
#endif

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif


#ifndef Q_OS_ANDROID
#include "qompnetworkingfactory.h"
//#include <QUuid>

static const QString TrackingID = "UA-42858086-4";

class QompGA : public QObject
{
	Q_OBJECT
public:
	explicit QompGA(QObject *p = nullptr) : QObject(p),
		analytics(new GAnalytics(TrackingID, this))
	{
		analytics->setNetworkAccessManager(QompNetworkingFactory::instance()->getMainNAM());
//		if(analytics->userID().isEmpty()) {
//			analytics->setUserID(QUuid::createUuid().toString());
//		}
#ifdef DEBUG_OUTPUT
		analytics->setLogLevel(GAnalytics::Debug);
#endif
		analytics->startSession();
	}

	~QompGA()
	{
		analytics->endSession();
	}

	GAnalytics* analytics;
};

QompGA* qompGA = nullptr;
#endif

namespace Qomp {

QString encodePassword(const QString &pass, const QString &key)
{
	QString result;
	int n1, n2;

	if (key.length() == 0) {
		return pass;
	}

	for (n1 = 0, n2 = 0; n1 < pass.length(); ++n1) {
		ushort x = pass.at(n1).unicode() ^ key.at(n2++).unicode();
		QString hex;
		hex.sprintf("%04x", x);
		result += hex;
		if(n2 >= key.length()) {
			n2 = 0;
		}
	}
	return result;
}

QString decodePassword(const QString &pass, const QString &key)
{
	QString result;
	int n1, n2;

	if (key.length() == 0) {
		return pass;
	}

	for(n1 = 0, n2 = 0; n1 < pass.length(); n1 += 4) {
		ushort x = 0;
		if(n1 + 4 > pass.length()) {
			break;
		}
		x += QString(pass.at(n1)).toInt(NULL,16)*4096;
		x += QString(pass.at(n1+1)).toInt(NULL,16)*256;
		x += QString(pass.at(n1+2)).toInt(NULL,16)*16;
		x += QString(pass.at(n1+3)).toInt(NULL,16);
		QChar c(x ^ key.at(n2++).unicode());
		result += c;
		if(n2 >= key.length()) {
			n2 = 0;
		}
	}
	return result;
}


QString durationSecondsToString(uint sec)
{
	int h = sec / (60*60);
	sec -= h*(60*60);
	int m = sec / 60;
	sec -= m*60;
	QTime time(h, m, sec, 0);
	const QString format = h ? "hh:mm:ss" : "mm:ss";
	return time.toString(format);
}

QString durationMiliSecondsToString(qint64 ms)
{
	int h = ms / (1000*60*60);
	ms -= h*(1000*60*60);
	int m = ms / (1000*60);
	ms -= m*(1000*60);
	int s = ms / 1000;
	ms -= s*1000;
	QTime t(h, m, s, ms);
	const QString format = h ? "hh:mm:ss" : "mm:ss";
	return t.toString(format);
}

uint durationStringToSeconds(const QString& dur)
{
	uint total = 0;
	int factor = 1;
	QStringList parts = dur.split(":");
	while(!parts.isEmpty()) {
		total += parts.takeLast().toInt()*factor;
		factor *= 60;
	}
	return total;
}

QString unescape(const QString& escaped)
{
	QTextDocument doc;
	doc.setHtml(escaped);
//	QString plain = escaped;
//	plain.replace("&lt;", "<");
//	plain.replace("&gt;", ">");
//	plain.replace("&quot;", "\"");
//	plain.replace("&amp;", "&");
//	plain.replace("&#39;", "'");
	return doc.toPlainText();
}

QString cacheDir()
{
	QString dir;
#ifdef Q_OS_ANDROID
	dir = QString("%1/.%2").arg(Qomp::storageDir(), qApp->organizationName());
#else
	QStringList list = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if(!list.isEmpty())
		dir = list.first();
	else
		dir = QDir::homePath();
#endif
	QDir d(dir);
	if(!d.exists())
		d.mkpath(dir);

	return dir;
}

QString dataDir()
{
	QString dir;

	QStringList list = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
	if(!list.isEmpty())
		dir = list.first();
	else
		dir = QDir::homePath();

	QDir d(dir);
	if(!d.exists())
		d.mkpath(dir);

	return dir;
}

bool checkIsUTF(const QByteArray &data)
{
	QTextCodec::ConverterState state;
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	codec->toUnicode(data.constData(), data.size(), &state);
	return (state.invalidChars == 0);
}

#ifdef QOMP_MOBILE
QString safeDir(const QString& dir)
{
	QFileInfo info(dir);
	if(!dir.isEmpty() && info.exists() && info.isDir())
		return info.absoluteFilePath();

	static const QString defDir(Qomp::storageDir() + "/");
	return defDir;
}

QString storageDir() {
	QAndroidJniObject storage = QAndroidJniObject::callStaticObjectMethod("android.os.Environment",
								       "getExternalStorageDirectory",
								       "()Ljava/io/File;");
	auto path = storage.callObjectMethod("getAbsolutePath", "()Ljava/lang/String;");
	return path.toString();
}
#endif

void logEvent(const QString &name, const QMap<QString, QString> &data)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "logEvent" << name << data;
#endif
#ifdef Q_OS_ANDROID
	QAndroidJniEnvironment env;

	static jclass javaArrayList  = static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
	static jmethodID javaInitArrayList = env->GetMethodID(javaArrayList, "<init>", "(I)V");

	auto eventName = QAndroidJniObject::fromString(name);
	//jmethodID javaArrayListAdd  = env->GetMethodID(java_util_ArrayList, "add", "(Ljava/lang/String;)Z");

	jobject stringArrayKeys = env->NewObject(javaArrayList, javaInitArrayList, data.keys().size());
	jobject stringArrayValues = env->NewObject(javaArrayList, javaInitArrayList, data.keys().size());
	auto keys = QAndroidJniObject::fromLocalRef(stringArrayKeys);
	auto values = QAndroidJniObject::fromLocalRef(stringArrayValues);

	foreach(const QString& key, data.keys()) {
		auto k = QAndroidJniObject::fromString(key);
		auto v = QAndroidJniObject::fromString(data.value(key));
		keys.callMethod<jboolean>("add", "(Ljava/lang/Object;)Z", k.object<jstring>());
		values.callMethod<jboolean>("add", "(Ljava/lang/Object;)Z", v.object<jstring>());
	}

	QtAndroid::androidActivity().callMethod<void>("logEvent", "(Ljava/lang/String;Ljava/util/ArrayList;Ljava/util/ArrayList;)V",
						eventName.object<jstring>(),
						keys.object<jobject>(),
						values.object<jobject>());
#else
	if(!qompGA)
		qompGA = new QompGA(qApp);

	if(name == ITEM_VIEW_EVENT) {
		qompGA->analytics->sendScreenView(data[ITEM_VIEW_EVENT_DATA]);
	}
	else {
		QVariantMap map;
		for(QMap<QString,QString>::const_iterator it = data.constBegin(); it != data.constEnd(); ++it) {
			map[it.key()] = it.value();
		}
		qompGA->analytics->sendEvent(name, "", "", "", map);
	}
#endif
}

/**
 * Helper function for forceUpdate().
 */
#ifndef QOMP_MOBILE
static void invalidateLayout(QLayout *layout)
{
	static const QString mainWinLayoutName("QMainWindowLayout");
	const int cnt = mainWinLayoutName.compare(layout->metaObject()->className()) == 0
						? 1 : layout->count();
	for (int i = 0; i < cnt; i++) {
		QLayoutItem *item = layout->itemAt(i);
		if (item->layout()) {
			invalidateLayout(item->layout());
		} else {
			item->invalidate();
		}
	}
	layout->invalidate();
	layout->activate();
}

void forceUpdate(QWidget *widget)
{
	// Update all child widgets.
	for (int i = 0; i < widget->children().size(); i++) {
		QObject *child = widget->children()[i];
		if (child->isWidgetType()) {
			forceUpdate(static_cast<QWidget*>(child));
		}
	}

	// Invalidate the layout of the widget.
	if (widget->layout()) {
		invalidateLayout(widget->layout());
	}
}

QMainWindow *getMainWindow()
{
	for(QWidget* w: qApp->topLevelWidgets()) {
		QMainWindow *win = qobject_cast<QMainWindow*>(w);
		if(win)
			return win;
	}
	return nullptr;
}
#endif
} //namespace Qomp

#ifdef DEBUG_OUTPUT
QDebug operator<<(QDebug dbg, Qomp::State value)
{
	switch(value) {
	case Qomp::StateUnknown:
		dbg.nospace() << "Qomp::StateUnknown";
		break;
	case Qomp::StateStopped:
		dbg.nospace() << "Qomp::StateStopped";
		break;
	case Qomp::StatePaused:
		dbg.nospace() << "Qomp::StatePaused";
		break;
	case Qomp::StatePlaying:
		dbg.nospace() << "Qomp::StatePlaying";
		break;
	case Qomp::StateError:
		dbg.nospace() << "Qomp::StateError";
		break;
	case Qomp::StateLoading:
		dbg.nospace() << "Qomp::StateLoading";
		break;
	case Qomp::StateBuffering:
		dbg.nospace() << "Qomp::StateBuffering";
		break;
	}

	return dbg.space();
}
#endif

#ifndef Q_OS_ANDROID
#include "common.moc"
#endif
