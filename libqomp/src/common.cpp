/*
 * Copyright (C) 2013  Khryukin Evgeny
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
#include "options.h"
#include "tune.h"

#include <QTime>
#include <QTextDocument>
#include <QStandardPaths>
#include <QDir>
#include <QTextCodec>
#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#else
#include <QWidget>
#include <QLayout>
#include <QApplication>
#endif

#ifndef Q_OS_MAC
#include <taglib/tstring.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/mpegfile.h>
#else
#include <tag/tstring.h>
#include <tag/id3v2tag.h>
#include <tag/attachedpictureframe.h>
#include <tag/mpegfile.h>
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
	dir = QString("/sdcard/.%1").arg(qApp->organizationName());
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

#ifdef QOMP_MOBILE
QString safeDir(const QString& dir)
{
	QFileInfo info(dir);
	if(!dir.isEmpty() && info.exists() && info.isDir())
		return info.absoluteFilePath();

	static const QString defDir("/sdcard/");
	return defDir;
}

#endif

QString safeTagLibString2QString(const TagLib::String& string)
{
	QString ret;

	if(string.isAscii()) {
		ret = QString::fromLatin1(string.toCString(false));
	}
	else if(!string.isLatin1()) {
		ret = QString::fromUtf8(string.toCString(true));
	}
	else {
		QByteArray ba(string.toCString(false));
		ret = QString(ba);

		const QByteArray decoding = Options::instance()->getOption(OPTION_DEFAULT_ENCODING).toByteArray();
		QTextCodec *tc = QTextCodec::codecForName(decoding);
		if(tc) {
			ret = tc->toUnicode(ba);
		}
	}

	return ret;
}

static bool searchLocalCover(Tune* t)
{
	if(t->file.isEmpty())
		return false;

	static const QStringList coverList{"front", "cover", "albumart"};
	static const QStringList coverExt{"jpg", "jpeg", "png"};
	for(const QString& cover: coverList)
		for(const QString& ext: coverExt) {
			const QString file = QFileInfo(t->file).absolutePath() + "/" + cover + "." + ext;
			if(QFile::exists(file)) {
				QImage i(file);
				if(!i.isNull()) {
					t->setCover(i);
					return true;
				}
			}
		}

	return false;
}

void loadCover(Tune *tune, TagLib::File *file)
{
	if (!searchLocalCover(tune)) {
		auto mpeg = dynamic_cast<TagLib::MPEG::File*>(file);

		if(mpeg) {
			TagLib::ID3v2::Tag* tag2 = mpeg->ID3v2Tag();
			if(tag2) {
				TagLib::ID3v2::FrameList frameList = tag2->frameList("APIC");
				if(!frameList.isEmpty()) {
					for(unsigned int i = 0; i < frameList.size(); ++i) {
						TagLib::ID3v2::Frame* pic = frameList[i];
						TagLib::ID3v2::AttachedPictureFrame *coverImg = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(pic);
						if(coverImg) {
							QImage cover;
							if(cover.loadFromData((const uchar *) coverImg->picture().data(), coverImg->picture().size())) {
								tune->setCover(cover);
								break;
							}
						}
					}
				}
			}
		}
	}
}

TagLib::FileName fileName2TaglibFileName(const QString &file)
{
	TagLib::String str( file.toUtf8().constData(), TagLib::String::UTF8 );
#ifdef Q_OS_WIN
	TagLib::FileName fname(str.toCWString());
#else
	TagLib::FileName fname(str.toCString(true));
#endif
	return fname;
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
