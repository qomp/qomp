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
#elif defined HAVE_H11
#elif defined Q_OS_WIN
#include <QApplication>
#include <QSystemTrayIcon>
#endif

class NotificationsPlugin::Private
{
public:
	Private(NotificationsPlugin* plugin) :
		plugin_(plugin),
		tune_(0)
	{
	}

	void showNotification(const QString& text)
	{
#ifdef Q_OS_ANDROID
		QAndroidJniObject str = QAndroidJniObject::fromString(text);
		QAndroidJniObject::callStaticMethod<void>("com/googlecode/qomp/Qomp",
								"showNotification",
								"(Ljava/lang/String;)V",
								str.object<jstring>());
#elif defined Q_OS_WIN
		QSystemTrayIcon* ico = qApp->findChild<QSystemTrayIcon*>();
		ico->showMessage(APPLICATION_NAME, text, QSystemTrayIcon::Information, 5000);
#else
		Q_UNUSED(text)
#endif
	}

public:
	NotificationsPlugin* plugin_;
	Tune* tune_;
};




NotificationsPlugin::NotificationsPlugin() : enabled_(false)
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
	player_ = player;
	connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStatusChanged(Qomp::State)));
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
	if(!enabled_)
		return;

	if(state == Qomp::StatePlaying) {
		Tune* t = player_->currentTune();
		if(t != d->tune_) {
			d->tune_ = t;
			d->showNotification(t->displayString());
		}
	}
}

void NotificationsPlugin::init()
{
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(notificationsplugin, NotificationsPlugin)
#endif
