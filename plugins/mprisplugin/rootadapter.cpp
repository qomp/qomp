/*
 * Copyright (C) 2016-2019  Khryukin Evgeny, Vitaly Tonkacheyev
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
#include "mpriscontroller.h"

#include <QDBusConnection>
#include <QDBusMessage>

RootAdapter::RootAdapter(MprisController *p)
 :QDBusAbstractAdaptor(p),
  controller_(p)
{
}

void RootAdapter::setData()
{
	QVariantMap map({{"SupportedMimeTypes", getMimeTypes()},
			 {"Identity", getIdentity()},
			 {"CanQuit", canQuit()},
			 {"CanRaise", canRaise()},
			 {"CanSetFullscreen", canSetFullscreen()},
			 {"HasTrackList", hasTrackList()}});
	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
						      "org.freedesktop.DBus.Properties",
						      "PropertiesChanged");
	msg << "org.mpris.MediaPlayer2" << map << QStringList();
	QDBusConnection::sessionBus().send(msg);
}

void RootAdapter::Quit()
{
	if(canQuit()) {
		controller_->emitSignal(QUIT);
	}
}

void RootAdapter::Raise()
{
	if(canRaise()) {
		controller_->emitSignal(RAISE);
	}
}

QStringList RootAdapter::getMimeTypes() const
{
	return QStringList({"audio/aac", "audio/x-flac",
			     "audio/flac", "audio/mp3",
			     "audio/mpeg", "application/ogg",
			     "audio/x-vorbis+ogg", "audio/x-ms-wma",
			     "audio/mp4", "audio/MP4A-LATM",
			     "audio/mpeg4-generic", "audio/m4a",
			     "audio/ac3"});
}
