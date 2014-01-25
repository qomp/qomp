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

#include "qompqtmultimediaplayer.h"
#include "qompqtmultimediametadataresolver.h"

#include <QMediaPlayer>
#include <QMediaContent>
#include <QAudioDeviceInfo>

QompQtMultimediaPlayer::QompQtMultimediaPlayer(QObject *parent) :
	QompPlayer(parent),
	player_(new QMediaPlayer(this)),
	resolver_(new QompQtMultimediaMetaDataResolver())
{
	connect(player_, SIGNAL(positionChanged(qint64)), SIGNAL(currentPositionChanged(qint64)));
	connect(player_, SIGNAL(volumeChanged(int)), SLOT(volumeChanged(int)));
	connect(player_, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
	connect(player_, SIGNAL(durationChanged(qint64)), SIGNAL(currentTuneTotalTimeChanged(qint64)));
	connect(player_, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QMediaPlayer::State)));
	connect(player_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

	connect(resolver_, SIGNAL(newMetaData(Tune,QMap<QString,QString>)), SIGNAL(newMetaData(Tune,QMap<QString,QString>)));
	connect(resolver_, SIGNAL(newDuration(Tune,qint64)), SIGNAL(newDuration(Tune,qint64)));
}

QompQtMultimediaPlayer::~QompQtMultimediaPlayer()
{
	delete resolver_;
}

void QompQtMultimediaPlayer::doSetTune(const Tune &tune)
{
	player_->setMedia(QMediaContent(tune.getUrl()));
}

QompMetaDataResolver *QompQtMultimediaPlayer::metaDataResolver() const
{
	return resolver_;
}

void QompQtMultimediaPlayer::setVolume(qreal vol)
{
	player_->blockSignals(true);
	player_->setVolume(vol*100);
	player_->blockSignals(false);
}

qreal QompQtMultimediaPlayer::volume() const
{
	return qreal(player_->volume())/100;
}

void QompQtMultimediaPlayer::setMute(bool mute)
{
	player_->blockSignals(true);
	player_->setMuted(mute);
	player_->blockSignals(false);
}

bool QompQtMultimediaPlayer::isMuted() const
{
	return player_->isMuted();
}

void QompQtMultimediaPlayer::setPosition(qint64 pos)
{
	player_->blockSignals(true);
	player_->setPosition(pos);
	player_->blockSignals(false);
}

qint64 QompQtMultimediaPlayer::position() const
{
	return player_->position();
}

QompPlayer::State QompQtMultimediaPlayer::state() const
{
	switch(player_->state()) {
	case QMediaPlayer::StoppedState:
		return QompPlayer::StateStopped;
	case QMediaPlayer::PlayingState:
		return QompPlayer::StatePlaying;
	case QMediaPlayer::PausedState:
		return QompPlayer::StatePaused;
	}
	return QompPlayer::StateUnknown;
}

void QompQtMultimediaPlayer::play()
{
	player_->play();
}

void QompQtMultimediaPlayer::pause()
{
	player_->pause();
}

void QompQtMultimediaPlayer::stop()
{
	player_->stop();
}

qint64 QompQtMultimediaPlayer::currentTuneTotalTime() const
{
	return player_->duration();
}

QStringList QompQtMultimediaPlayer::audioOutputDevice() const
{
	QStringList list;
//	foreach(const QAudioDeviceInfo& info, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)) {
//		list.append(info.deviceName());
//	}

	return list;
}

void QompQtMultimediaPlayer::setAudioOutputDevice(const QString &/*devName*/)
{
//	if(devName.isEmpty()) {
//		QAudioDeviceInfo::defaultOutputDevice()
//	}
}

void QompQtMultimediaPlayer::volumeChanged(int vol)
{
	emit volumeChanged(qreal(vol)/100);
}

void QompQtMultimediaPlayer::stateChanged(QMediaPlayer::State state)
{
	if(state == QMediaPlayer::StoppedState && position() == currentTuneTotalTime())
		emit mediaFinished();

	emit QompPlayer::stateChanged(this->state());
}

void QompQtMultimediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
	switch(status) {
	case QMediaPlayer::LoadingMedia:
		emit QompPlayer::stateChanged(QompPlayer::StateLoading);
		break;
	case QMediaPlayer::BufferingMedia:
		emit QompPlayer::stateChanged(QompPlayer::StateBuffering);
		break;
	default:
		stateChanged(player_->state());
		break;
	}
}

