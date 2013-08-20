#include "mprisplugin.h"
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

mprisplugin::mprisplugin(QObject *parent)
{
	QDBusConnection qompConnection = QDBusConnection::sessionBus();
	qompConnection.registerObject("/org/mpris/MediaPlayer2", this);
	qompConnection.registerService("org.mpris.MediaPlayer2.qomp");
	statusChanged = false;
	metadataChanged = false;
	playerStatus = "Stopped"; //default on start
}

mprisplugin::~mprisplugin()
{
	QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.qomp");
}

mprisplugin::setStatus(const QString &status)
{
	if (!status.isEmpty() && status != playerStatus) {
		playerStatus = status; //"Playing" or "Stopped"
		statusChanged = true;
	}
	else {
		statusChanged = false;
	}
}

void mprisplugin::setMetadata(int trackNumber, const QString &title, const QString &artist, const QString &album, const QString &url)
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

QVariantMap mprisplugin::metadata()
{
	return metaData;
}

QString mprisplugin::playbackStatus()
{
	return playerStatus;
}

void mprisplugin::sendProperties()
{
	QVariantMap map;
	if (!playerStatus.isEmpty() && statusChanged) {
		map.insert("playbackStatus", playbackStatus());
	}
	if (!metadata().isEmpty() && metadataChanged) {
		map.insert("metadata", metadata());
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
