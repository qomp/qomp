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
#include "yandexmusicgettunsdlg.h"
#include "yandexmusicurlresolvestrategy.h"

#include <QtPlugin>

YandexMusicPlugin::YandexMusicPlugin(QObject *parent) :
	QObject(parent)
{
}

QString YandexMusicPlugin::name() const
{
	return "Yandex.Music plugin";
}

QString YandexMusicPlugin::version() const
{
	return "0.1";
}

QString YandexMusicPlugin::description() const
{
	return "Listening to the music from Yandex.Music";
}

TuneList YandexMusicPlugin::getTunes()
{
	TuneList list;
	YandexMusicGettunsDlg dlg;
	dlg.show();
	if(dlg.exec() == QDialog::Accepted) {
		list = dlg.getTunes();
	}
	return list;
}

QompOptionsPage *YandexMusicPlugin::options()
{
	return 0;
}

TuneURLResolveStrategy *YandexMusicPlugin::urlResolveStrategy() const
{
	return YandexMusicURLResolveStrategy::instance();
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(yandexmusicplugin, YandexMusicPlugin)
#endif