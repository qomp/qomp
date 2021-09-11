/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "qompqmlengine.h"
#include "scaler.h"
#include "covercache.h"
#include "common.h"
#include "defines.h"

#include <QGuiApplication>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickWindow>
#include <QQuickImageProvider>
#include <QPixmapCache>
#include <QTimer>

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QKeyEvent>

QQuickWindow* _instance = nullptr;

static const int UPDATE_INTERVAL = 100;
static const char* PROP_URL = "prop_url";

#ifdef Q_OS_ANDROID
static void menuKeyDown(JNIEnv */*env*/, jobject /*thiz*/)
{
	qApp->postEvent(_instance->activeFocusItem(), new QKeyEvent(QEvent::KeyRelease, Qt::Key_Menu, Qt::NoModifier));//QMetaObject::invokeMethod(_instance, "doMainMenu");
}
#endif


class QompTuneCoveImageProvider : public QQuickImageProvider
{
public:
	QompTuneCoveImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Pixmap){}

	virtual QPixmap	requestPixmap(const QString &id, QSize */*size*/, const QSize &/*requestedSize*/)
	{
		const QImage img = CoverCache::instance()->get(id);
		QPixmap pix = QPixmap::fromImage(img);
//		*size = pix.size();
		return pix;
	}

	static const QString& name()
	{
		static const QString n = QStringLiteral("tune");
		return n;
	}
};


QompQmlEngine *QompQmlEngine::instance()
{
	if(!instance_)
		instance_ = new QompQmlEngine;

	return instance_;
}

QompQmlEngine::~QompQmlEngine()
{
	removeEventFilter(this);
	QMetaObject::invokeMethod(window_, "beforeClose");
	window_->update();
	qApp->processEvents();
	removeImageProvider(QompTuneCoveImageProvider::name());
	delete scaler_;
//	clearComponentCache();
//	collectGarbage();
//	qApp->processEvents();
}

QQuickItem *QompQmlEngine::createItem(const QUrl &url)
{
	QQmlComponent* comp = new QQmlComponent(this, url);
	QQuickItem* item = static_cast<QQuickItem*>(comp->create(rootContext()));
	comp->setParent(item);
	connect(item, SIGNAL(destroyed()), SLOT(itemDeleted()));
	item->setProperty(PROP_URL, url.toDisplayString());
	return item;
}

void QompQmlEngine::addItem(QQuickItem *item)
{
	item->setParent(0);
	setObjectOwnership(item, QQmlEngine::JavaScriptOwnership);
	QMetaObject::invokeMethod(window_, "addView", Qt::DirectConnection,
				  Q_ARG(QVariant, QVariant::fromValue(item)));
	Qomp::logEvent(ITEM_VIEW_EVENT, {{"qml_item", item->property(PROP_URL).toString()}});
}

void QompQmlEngine::removeItem()
{
	QMetaObject::invokeMethod(window_, "removeView", Qt::DirectConnection);
}

void QompQmlEngine::itemDeleted()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQmlEngine::itemDeleted() " << sender()->metaObject()->className();
#endif
}

bool QompQmlEngine::eventFilter(QObject *o, QEvent *e)
{
	if(o == qApp) {
		if(e->type() == QEvent::ApplicationStateChange) {
			QTimer::singleShot(UPDATE_INTERVAL, this, SLOT(updateMainWindowState()));
		}
	}
	return QQmlApplicationEngine::eventFilter(o, e);
}

void QompQmlEngine::showConfirmExitMessage()
{
	QAndroidJniObject str = QAndroidJniObject::fromString(tr("Press again to exit"));
	QAndroidJniObject act = QtAndroid::androidActivity();
	act.callMethod<void>("showNotification","(Ljava/lang/String;)V", str.object<jstring>());
}

void QompQmlEngine::updateMainWindowState()
{
	Qt::ApplicationState state = qApp->applicationState();
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQmlEngine::eventFilter() " << state;
#endif
	QWindow::Visibility vis = (QWindow::Visibility)window_->property("visibility").toInt();

	if(state == Qt::ApplicationActive) {
		if(vis != QWindow::Maximized) {
			window_->setProperty("visibility", QWindow::Maximized);
			window_->update();
		}
	}
	else {
		if(vis != QWindow::Hidden)
			window_->setProperty("visibility", QWindow::Hidden);
	}
}

QompQmlEngine::QompQmlEngine() :
	QQmlApplicationEngine(qApp),
	window_(0),
	scaler_(new Scaler)
{
	qmlRegisterType<Scaler>("net.sourceforge.qomp", 1, 0, "Scaler");
	addImageProvider(QompTuneCoveImageProvider::name(), new QompTuneCoveImageProvider);
	rootContext()->setContextProperty("scaler", scaler_);

	load(QUrl("qrc:///qmlshared/QompAppWindow.qml"));
	window_ = static_cast<QQuickWindow*>(rootObjects().first());
	_instance = window_;
	connect(window_, SIGNAL(confirmExit()), SLOT(showConfirmExitMessage()));

#ifdef Q_OS_ANDROID
	QAndroidJniObject act = QtAndroid::androidActivity();
	QAndroidJniEnvironment jni;
	jclass clazz = jni->GetObjectClass(act.object());
	JNINativeMethod methods[] = {{ "menuKeyDown", "()V", (void*)menuKeyDown }};
	jni->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
	jni->DeleteLocalRef(clazz);
#endif
	window_->showMaximized();
	window_->update();
	qApp->processEvents();
	qApp->installEventFilter(this);
}

QompQmlEngine* QompQmlEngine::instance_ = 0;
