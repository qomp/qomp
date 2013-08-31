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
