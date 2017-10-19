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

#include "yandexmusicplugin.h"
#include "yandexmusiccontroller.h"
#include "yandexmusicurlresolvestrategy.h"
#include "qomppluginaction.h"

#include <QtPlugin>

YandexMusicPlugin::YandexMusicPlugin(QObject *parent) :
	QObject(parent)
{
}

QString YandexMusicPlugin::description() const
{
	return tr("Listening to the music from Yandex.Music");
}

void YandexMusicPlugin::getTunes(QompPluginAction *act)
{
	YandexMusicController* ctrl = new YandexMusicController(this);
	connect(ctrl, &YandexMusicController::tunesReady, act, &QompPluginAction::setTunesReady);
	ctrl->getTunes();
}

QompOptionsPage *YandexMusicPlugin::options()
{
	return 0;
}

TuneURLResolveStrategy *YandexMusicPlugin::urlResolveStrategy() const
{
	return YandexMusicURLResolveStrategy::instance();
}

void YandexMusicPlugin::unload()
{
	YandexMusicURLResolveStrategy::reset();
}

QList<QompPluginAction *> YandexMusicPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;
	QompPluginAction *act = new QompPluginAction(QIcon(), tr("Yandex.Music"), this, "getTunes", this);
	l.append(act);
	return l;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(yandexmusicplugin, YandexMusicPlugin)
#endif
