/*
 * Copyright (C) 2013  Khryukin Evgeny, Vitaly Tonkacheyev
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

#include "mpris.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QStringList>

Mpris::Mpris(QObject* p) :
	QDBusAbstractAdaptor(p)
{
	statusChanged = false;
	metadataChanged = false;
	playerStatus = "Stopped"; //default on start
}


void Mpris::setStatus(const QString &status)
{
	if (!status.isEmpty() && status != playerStatus) {
		playerStatus = status; //"Playing" or "Stopped"
		statusChanged = true;
	}
	else {
		statusChanged = false;
	}
}

void Mpris::setMetadata(int trackNumber, const QString &title, const QString &artist, const QString &album, const QString &url)
{
	QVariantMap map;
	if (!album.isEmpty()) {
		map["xesam:album"] = album;
	}
	if (!artist.isEmpty()) {
		map["xesam:artist"] = QStringList() << artist;
	}
	if (!title.isEmpty()) {
		map["xesam:title"] = QStringList() << title;
	}
	map["xesam:trackNumber"] = trackNumber;
	map["xesam:url"] = url;
	if (!map.isEmpty() && map != metaData) {
		metaData = map;
		metadataChanged = true;
	}
	else {
		metadataChanged = false;
	}

}

QVariantMap Mpris::metadata() const
{
	return metaData;
}

QString Mpris::playbackStatus() const
{
	return playerStatus;
}

void Mpris::sendProperties()
{
	QVariantMap map;
	if (!playerStatus.isEmpty() && statusChanged) {
        map.insert("PlaybackStatus", playbackStatus());
	}
	if (!metadata().isEmpty() && metadataChanged) {
        map.insert("Metadata", metadata());
	}
	if (map.isEmpty()) {
		return;
	}
	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
						      "org.freedesktop.DBus.Properties", "PropertiesChanged");
	QVariantList args = QVariantList() << "org.mpris.MediaPlayer2.Player" << map << QStringList();
	msg.setArguments(args);
	QDBusConnection::sessionBus().send(msg);
}
