/*
 * Copyright (C) 2016  Khryukin Evgeny, Vitaly Tonkacheyev
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
#include "rootadapter.h"
#include <QDBusConnection>
#include <QDBusMessage>

RootAdapter::RootAdapter(QObject *p):QDBusAbstractAdaptor(p)
{
}

void RootAdapter::setData()
{
	QVariantMap map;
	map.insert("SupportedMimeTypes", getMimeTypes());
	map.insert("Identity", getIdentity());
	map.insert("CanQuit", canQuit());
	map.insert("CanRaise", canRaise());
	map.insert("CanSetFullscreen", canSetFullscreen());
	map.insert("HasTrackList", hasTrackList());
	if (map.isEmpty()) {
		return;
	}
	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
	                                              "org.freedesktop.DBus.Properties",
	                                              "PropertiesChanged");
	QVariantList args = QVariantList() << "org.mpris.MediaPlayer2" << map << QStringList();
	msg.setArguments(args);
	QDBusConnection::sessionBus().send(msg);
}
