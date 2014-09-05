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

#include <QCoreApplication>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickWindow>
#include <QQuickImageProvider>
#include <QPixmapCache>

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QKeyEvent>

QQuickWindow* _instance = nullptr;

#ifdef Q_OS_ANDROID
static void menuKeyDown(JNIEnv */*env*/, jobject /*thiz*/)
{
	qApp->postEvent(_instance->activeFocusItem(), new QKeyEvent(QEvent::KeyRelease, Qt::Key_Menu, Qt::NoModifier));//QMetaObject::invokeMethod(_instance, "doMainMenu");
}
#endif

//class QompImageProvider : public QQuickImageProvider
//{
//public:
//	QompImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Pixmap){}

//	virtual QPixmap	requestPixmap(const QString &id, QSize */*size*/, const QSize &/*requestedSize*/)
//	{
//		QPixmap pix;
//		if(QPixmapCache::find(id, &pix))
//			return pix.copy();

//		return QPixmap();
//	}

//	static const QString& name()
//	{
//		static const QString n = QStringLiteral("qomp");
//		return n;
//	}
//};


QompQmlEngine *QompQmlEngine::instance()
{
	if(!instance_)
		instance_ = new QompQmlEngine;

	return instance_;
}

QompQmlEngine::~QompQmlEngine()
{	
	QMetaObject::invokeMethod(window_, "beforeClose");
	window_->update();
	qApp->processEvents();
	//removeImageProvider(QompImageProvider::name());
	delete scaler_;
	clearComponentCache();
	collectGarbage();
	qApp->processEvents();
}

QQuickItem *QompQmlEngine::createItem(const QUrl &url)
{
	QQmlComponent* comp = new QQmlComponent(this, url);
	QQuickItem* item = static_cast<QQuickItem*>(comp->create(rootContext()));
	comp->setParent(item);
	connect(item, SIGNAL(destroyed()), SLOT(itemDeleted()));
	return item;
}

void QompQmlEngine::addItem(QQuickItem *item)
{
	item->setParent(0);
	setObjectOwnership(item, QQmlEngine::JavaScriptOwnership);
	QMetaObject::invokeMethod(window_, "addView", Qt::DirectConnection,
				  Q_ARG(QVariant, QVariant::fromValue(item)));
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
		if(e->type() == QEvent::ApplicationActivate)
			window_->setProperty("visible", true);
		else if(e->type() == QEvent::ApplicationDeactivate)
			window_->setProperty("visible", false);
	}
	return QQmlApplicationEngine::eventFilter(o, e);
}

QompQmlEngine::QompQmlEngine() :
	QQmlApplicationEngine(qApp),
	window_(0),
	scaler_(new Scaler)
{
	//addImageProvider(QompImageProvider::name(), new QompImageProvider);
	rootContext()->setContextProperty("scaler", scaler_);

	load(QUrl("qrc:///qmlshared/QompAppWindow.qml"));
	window_ = static_cast<QQuickWindow*>(rootObjects().first());
	_instance = window_;

#ifdef Q_OS_ANDROID
	QAndroidJniObject act = QtAndroid::androidActivity();
	QAndroidJniEnvironment jni;
	jclass clazz = jni->GetObjectClass(act.object());
	JNINativeMethod methods[] = { "menuKeyDown", "()V", (void*)menuKeyDown };
	jni->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
	jni->DeleteLocalRef(clazz);
#endif
	window_->showMaximized();
	window_->update();
	qApp->processEvents();
	qApp->installEventFilter(this);
}

QompQmlEngine* QompQmlEngine::instance_ = 0;
