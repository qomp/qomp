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

#include "notificationsplugin.h"
#include "options.h"
#include "tune.h"
#include "defines.h"

#include <QtPlugin>
#include <QTimer>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QtAndroid>
#elif defined HAVE_X11
#include <QImage>
#include "dbusnotifier.h"
#elif defined Q_OS_WIN
#include <QApplication>
#include <QSystemTrayIcon>
#elif defined Q_OS_MAC
#include"growlnotifier.h"

static const QString notificationName = QObject::tr("New Track");
#endif

class NotificationsPlugin::Private
{
public:
	explicit Private(NotificationsPlugin* plugin) :
		plugin_(plugin),
		tune_(0)
	{
#ifdef Q_OS_MAC
		growl_ = new GrowlNotifier(QStringList() << notificationName,
					   QStringList() << notificationName, APPLICATION_NAME);
#elif defined HAVE_X11
		dbusNotify_ = new DBusNotifier();
#endif
	}

	~Private()
	{
#ifdef Q_OS_MAC
		delete growl_;
#elif defined HAVE_X11
		delete dbusNotify_;
#endif
	}

	void showNotification(const QString& text, QImage art = QImage())
	{
		static const QString title = QString(APPLICATION_NAME).left(1).toUpper() + QString(APPLICATION_NAME).mid(1) + QObject::tr(" now playing:");
#ifdef Q_OS_ANDROID
		Q_UNUSED(art)
		Q_UNUSED(title)
		QAndroidJniObject str = QAndroidJniObject::fromString(text);
		QAndroidJniObject act = QtAndroid::androidActivity();
		act.callMethod<void>("showNotification", "(Ljava/lang/String;)V", str.object<jstring>());
#elif defined Q_OS_WIN
		QSystemTrayIcon* ico = qApp->findChild<QSystemTrayIcon*>();
		if(!art.isNull()) {
			ico->showMessage(title, text, QIcon(QPixmap::fromImage(art)), 5000);
		} else {
			ico->showMessage(title, text, QSystemTrayIcon::Information, 5000);
		}
#elif defined Q_OS_MAC
		static const QPixmap pix("qrc:///icons/icons/qomp.png");
		growl_->notify(notificationName, APPLICATION_NAME, text, pix);
#elif defined HAVE_X11
		if (dbusNotify_->isAvailable()) {
			QImage imageArt = (!art.isNull()) ? art : QImage(":/icons/icons/qomp.png");
			dbusNotify_->doPopup(title, text, imageArt);
		}
#endif
	}

public:
	NotificationsPlugin* plugin_;
	Tune* tune_;
#ifdef Q_OS_MAC
	GrowlNotifier* growl_;
#elif defined HAVE_X11
	DBusNotifier* dbusNotify_;
#endif
};




NotificationsPlugin::NotificationsPlugin() : player_(0), enabled_(false)
{
	d = new Private(this);
	QTimer::singleShot(0, this, SLOT(init()));
}

NotificationsPlugin::~NotificationsPlugin()
{
	delete d;
}

QompOptionsPage *NotificationsPlugin::options()
{
	return 0;
}

void NotificationsPlugin::qompPlayerChanged(QompPlayer *player)
{
	if(player_ != player) {
		if(player_)
			disconnect(player_, SIGNAL(stateChanged(Qomp::State)), this, SLOT(playerStatusChanged(Qomp::State)));

		player_ = player;
		if(player_)
			connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStatusChanged(Qomp::State)));
	}
}

void NotificationsPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void NotificationsPlugin::unload()
{
}

void NotificationsPlugin::playerStatusChanged(Qomp::State state)
{
	if(!enabled_ || !player_)
		return;

	if(state == Qomp::StatePlaying) {
		Tune* t = player_->currentTune();
		if(t != d->tune_) {
			d->tune_ = t;
			d->showNotification(t->displayString()
						#ifndef Q_OS_ANDROID
							, d->tune_->cover()
						#endif
					 );
		}
	}
}

void NotificationsPlugin::init()
{
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(notificationsplugin, NotificationsPlugin)
#endif
