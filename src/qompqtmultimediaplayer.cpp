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
//#include "qomptaglibmetadataresolver.h"
#include "tune.h"

#include <QMediaContent>
#include <QtConcurrent>
//#include <QAudioDeviceInfo>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

QompQtMultimediaPlayer::QompQtMultimediaPlayer() :
	QompPlayer(),
	player_(new QMediaPlayer(this)),
	resolver_(0/*new QompTagLibMetaDataResolver(this)*/),
	lastState_(Qomp::StateStopped),
	watcher_(0)
{
	connect(player_, SIGNAL(positionChanged(qint64)), SIGNAL(currentPositionChanged(qint64)));
	connect(player_, SIGNAL(volumeChanged(int)), SLOT(volumeChanged(int)));
	connect(player_, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
	connect(player_, SIGNAL(durationChanged(qint64)), SIGNAL(currentTuneTotalTimeChanged(qint64)));
	connect(player_, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(playerStateChanged(QMediaPlayer::State)));
	connect(player_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

	//connect(resolver_, SIGNAL(tuneUpdated(Tune*)), SIGNAL(tuneDataUpdated(Tune*)), Qt::QueuedConnection);
}

QompQtMultimediaPlayer::~QompQtMultimediaPlayer()
{
	//delete resolver_;
	delete watcher_;
}

void QompQtMultimediaPlayer::doSetTune()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::doSetTune()";
#endif
	if(!player_->media().isNull()) {
		player_->blockSignals(true);
		player_->setMedia(QMediaContent());
		player_->blockSignals(false);
	}

	if(watcher_) {
		delete watcher_;
	}
	watcher_ = new QFutureWatcher<QUrl>;
	connect(watcher_, SIGNAL(finished()), SLOT(tuneUrlReady()));
	Tune* t = currentTune();
	QFuture<QUrl> f = QtConcurrent::run(t, &Tune::getUrl);
	watcher_->setFuture(f);
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

static Qomp::State convertState(QMediaPlayer::State s)
{
	switch(s) {
	case QMediaPlayer::StoppedState:
		return Qomp::StateStopped;
	case QMediaPlayer::PlayingState:
		return Qomp::StatePlaying;
	case QMediaPlayer::PausedState:
		return Qomp::StatePaused;
	}
	return Qomp::StateUnknown;
}

Qomp::State QompQtMultimediaPlayer::state() const
{
	return convertState(player_->state());
}

void QompQtMultimediaPlayer::play()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::play()";
#endif
	if(!player_->media().isNull())
		player_->play();

	lastState_ = Qomp::StatePlaying;
}

void QompQtMultimediaPlayer::pause()
{
	player_->pause();
	lastState_ = Qomp::StatePaused;
}

void QompQtMultimediaPlayer::stop()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::stop()";
#endif
	player_->stop();
	lastState_ = Qomp::StateStopped;
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

void QompQtMultimediaPlayer::playerStateChanged(QMediaPlayer::State _state)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::stateChanged()  " << _state;
#endif
//	if(state == QMediaPlayer::StoppedState && position() == currentTuneTotalTime())
//		emit mediaFinished();

	emit stateChanged(convertState(_state));
}

void QompQtMultimediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::mediaStatusChanged()  " << status;
#endif
	switch(status) {
	case QMediaPlayer::LoadingMedia:
		emit stateChanged(Qomp::StateLoading);
		break;
	case QMediaPlayer::BufferingMedia:
		emit stateChanged(Qomp::StateBuffering);
		break;
	case QMediaPlayer::InvalidMedia:
		emit stateChanged(Qomp::StateError);
		break;
	case QMediaPlayer::EndOfMedia:
		emit mediaFinished();
		//break; we wont emit next  signal
	default:
		emit QompPlayer::stateChanged(state());
		break;
	}
}

void QompQtMultimediaPlayer::tuneUrlReady()
{
	QFuture<QUrl> f = watcher_->future();
	delete watcher_;
	watcher_ = 0;
	QUrl url = f.result();

#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::tuneUrlReady()  " << lastState_ << url;
#endif

	player_->setMedia(QMediaContent(url));

	switch (lastState_) {
	case Qomp::StatePlaying: player_->play();
		break;
	case Qomp::StatePaused: player_->pause();
		break;
	default: player_->stop();
		break;
	}	
}
