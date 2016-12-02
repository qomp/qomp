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
#include "signalhandler.h"

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
	msg << "org.mpris.MediaPlayer2" << map << QStringList();
	QDBusConnection::sessionBus().send(msg);
}

void RootAdapter::Quit()
{
	if(canQuit()) {
		SignalHandler::instance()->emitSignal(QUIT);
	}
}

void RootAdapter::Raise()
{
	if(canRaise()) {
		SignalHandler::instance()->emitSignal(RAISE);
	}
}

QStringList RootAdapter::getMimeTypes() const
{
	return QStringList() << "audio/aac" << "audio/x-flac"
			     << "audio/flac" << "audio/mp3"
			     << "audio/mpeg"<< "application/ogg"
			     << "audio/x-vorbis+ogg" << "audio/x-ms-wma"
			     << "audio/mp4" << "audio/MP4A-LATM"
			     << "audio/mpeg4-generic" << "audio/m4a"
			     << "audio/ac3";
}
