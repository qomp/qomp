/*
 * Copyright (C) 2013-2022  Khryukin Evgeny, Vitaly Tonkacheyev
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

#include "mprisadapter.h"
#include "mpriscontroller.h"
#include "tune.h"

#include <QStringList>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusObjectPath>
#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

MprisAdapter::MprisAdapter(MprisController *p) :
    QDBusAbstractAdaptor(p), controller_(p), playerStatus_("Stopped"), statusChanged_(false), metadataChanged_(false),
    shuffle_(false), loopAll_(QStringLiteral("None"))
{
}

void MprisAdapter::setStatus(const QString &status)
{
	if (!status.isEmpty() && (status != playerStatus_)) {
		playerStatus_  = status;
		statusChanged_ = true;
	}
	else {
		statusChanged_ = false;
	}
}

void MprisAdapter::setMetadata(const QompMetaData &tune)
{
	metaDataMap_.clear();
	if (!tune.title.isEmpty()) {
		metaDataMap_["xesam:title"] = tune.title;
	}
	else {
		return;
	}
	if (!tune.album.isEmpty()) {
		metaDataMap_["xesam:album"] = tune.album;
	}
	if (!tune.artist.isEmpty()) {
		metaDataMap_["xesam:artist"] = QStringList({ tune.artist });
	}
	metaDataMap_["xesam:url"]	  = tune.url;
	metaDataMap_["xesam:trackNumber"] = tune.trackNumber;
	metaDataMap_["mpris:length"]	  = tune.trackLength;
	quint32 argument		  = 0;
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
	argument = qrand();
#else
	argument = QRandomGenerator::global()->generate();
#endif
	trackId_		      = QDBusObjectPath(QString("/org/qomp/MediaPlayer2/Track/%1").arg(argument));
	metaDataMap_["mpris:trackid"] = QVariant::fromValue<QDBusObjectPath>(trackId_);
	if (!tune.cover.isEmpty())
		metaDataMap_["mpris:artUrl"] = tune.cover.startsWith("http") ? tune.cover : "file://" + tune.cover;
	metadataChanged_ = true;
}

QVariantMap MprisAdapter::metadata() const { return metaDataMap_; }

QString MprisAdapter::playbackStatus() const { return playerStatus_; }

void MprisAdapter::updateProperties()
{
	QVariantMap map({ { "CanGoNext", canGoNext() },
			  { "CanGoPrevious", canGoPrevious() },
			  { "CanPlay", canPlay() },
			  { "CanPause", canPause() },
			  { "CanSeek", canSeek() },
			  { "Volume", getVolume() },
			  { "Shuffle", shuffle() },
			  { "LoopStatus", loopAll() } });
	if (!playerStatus_.isEmpty() && statusChanged_)
		map.insert("PlaybackStatus", playbackStatus());

	if (!metadata().isEmpty() && metadataChanged_) {
		map.insert("Metadata", metadata());
		metadataChanged_ = false;
	}

	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2", "org.freedesktop.DBus.Properties",
						      "PropertiesChanged");
	msg << "org.mpris.MediaPlayer2.Player" << map << QStringList();
	QDBusConnection::sessionBus().send(msg);
}

void MprisAdapter::Play()
{
	if (canPlay())
		controller_->emitSignal(PLAY);
}

void MprisAdapter::Pause()
{
	if (canPause())
		controller_->emitSignal(PAUSE);
}

void MprisAdapter::Next()
{
	if (canGoNext())
		controller_->emitSignal(NEXT);
}

void MprisAdapter::Previous()
{
	if (canGoPrevious())
		controller_->emitSignal(PREVIOUS);
}

void MprisAdapter::PlayPause()
{
	if (canPlay() && canPause()) {
		if (playerStatus_ == "Playing")
			controller_->emitSignal(PAUSE);
		else
			controller_->emitSignal(PLAY);
	}
}

void MprisAdapter::Stop()
{
	if (canControl())
		controller_->emitSignal(STOP);
}

void MprisAdapter::setVolume(const qreal &volume)
{
	if (volume >= 0.0)
		controller_->emitSignal(VOLUME, volume);
}

void MprisAdapter::SetPosition(const QDBusObjectPath &TrackId, qlonglong Position)
{
	if (trackId_ == TrackId && Position > 0.0)
		controller_->emitSignal(POSITION, Position);
}

void MprisAdapter::setLoopAndShuffle(bool loop, bool shuffle)
{
	shuffle_ = shuffle;
	loopAll_ = (loop) ? QStringLiteral("Playlist") : QStringLiteral("None");
	updateProperties();
}

void MprisAdapter::sendShuffle(bool shuffle)
{
	shuffle_ = shuffle;
	controller_->emitSignal(SHUFFLE);
}

void MprisAdapter::sendLoopStatus(const QString &status)
{
	loopAll_ = status;
	controller_->emitSignal(LOOPALL);
}

qreal MprisAdapter::getVolume() { return controller_->getVolume(); }

qreal MprisAdapter::getPosition() { return controller_->getPosition(); }
