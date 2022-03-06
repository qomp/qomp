/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#include "qompqt6multimediaplayer.h"
#include "qomptaglibmetadataresolver.h"
#include "tune.h"
#include "gettuneurlhelper.h"

#include <QSignalBlocker>
#include <QCoreApplication>
#include <QAudioOutput>
#include <QAudioDevice>
#include <QMediaDevices>
//#include <QMetaDataReaderControl>


#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

QompQtMultimediaPlayer::QompQtMultimediaPlayer() :
	QompPlayer(),
	player_(new QMediaPlayer(this)),
	resolver_(new QompTagLibMetaDataResolver(this)),
	watcher_(0),
	prevTune_(Tune::emptyTune())
{
	player_->setAudioOutput(new QAudioOutput(player_));
	connect(player_->audioOutput(), &QAudioOutput::volumeChanged, this, &QompQtMultimediaPlayer::volumeChanged);
	connect(player_->audioOutput(), &QAudioOutput::mutedChanged,  this, &QompQtMultimediaPlayer::mutedChanged);

	connect(player_, &QMediaPlayer::playbackStateChanged,  this, &QompQtMultimediaPlayer::playerStateChanged);
	connect(player_, &QMediaPlayer::mediaStatusChanged, this, &QompQtMultimediaPlayer::mediaStatusChanged);
	connect(player_, &QMediaPlayer::durationChanged,    this, &QompQtMultimediaPlayer::tuneDurationChanged);
	connect(player_, &QMediaPlayer::positionChanged,    this, &QompQtMultimediaPlayer::tunePositionChanged);

#ifdef HAVE_X11
//	connect(player_, &QMediaPlayer::audioAvailableChanged, this, &QompQtMultimediaPlayer::audioReadyChanged);
	connect(player_, &QMediaPlayer::seekableChanged, this, &QompQtMultimediaPlayer::seekableChanged);
#endif
	connect(resolver_, &QompMetaDataResolver::tuneUpdated, this, &QompQtMultimediaPlayer::tuneDataUpdated, Qt::QueuedConnection);
}

QompQtMultimediaPlayer::~QompQtMultimediaPlayer()
{
}

void QompQtMultimediaPlayer::doSetTune()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::doSetTune()";
#endif
	if(!prevTune_ || !prevTune_->sameSource(currentTune())) {
		const QSignalBlocker b(player_);
		player_->setSource(QUrl());
		qApp->processEvents();
	}

	if(watcher_) {
		watcher_->parent()->setProperty("blocked", true);
	}

	GetTuneUrlHelper* helper = new GetTuneUrlHelper(this, "tuneUrlReady", this);
	watcher_ = helper->getTuneUrlAsynchronously(currentTune());
}

QompMetaDataResolver *QompQtMultimediaPlayer::metaDataResolver() const
{
	return resolver_;
}

qint64 QompQtMultimediaPlayer::mapPositionFromTune(qint64 pos) const
{
	if(currentTune())
		return currentTune()->start + pos;

	return pos;
}

qint64 QompQtMultimediaPlayer::mapPositionFromTrack(qint64 pos) const
{
	if(currentTune()) {
		qint64 cur = pos - currentTune()->start;
		return cur > 0 ? cur : 0;
	}

	return pos;
}

void QompQtMultimediaPlayer::setVolume(qreal vol)
{
	const QSignalBlocker b(this);
	player_->audioOutput()->setVolume(vol);
	qApp->processEvents();
}

qreal QompQtMultimediaPlayer::volume() const
{
	return player_->audioOutput()->volume();
}

void QompQtMultimediaPlayer::setMute(bool mute)
{
	const QSignalBlocker b(this);
	player_->audioOutput()->setMuted(mute);
	qApp->processEvents();
}

bool QompQtMultimediaPlayer::isMuted() const
{
	return player_->audioOutput()->isMuted();
}

void QompQtMultimediaPlayer::setPosition(qint64 pos)
{
	const QSignalBlocker b(player_);
	player_->setPosition( mapPositionFromTune(pos) );
	qApp->processEvents();
}

qint64 QompQtMultimediaPlayer::position() const
{
	return mapPositionFromTrack( player_->position() );
}

static Qomp::State convertState(QMediaPlayer::PlaybackState s)
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
	return convertState(player_->playbackState());
}

void QompQtMultimediaPlayer::play()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::play()";
#endif
	QompPlayer::play();

	if(!player_->source().isEmpty() && isTuneChangeFinished())
		doPlay();
}

void QompQtMultimediaPlayer::pause()
{
	QompPlayer::pause();

	player_->pause();
}

void QompQtMultimediaPlayer::stop()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::stop()";
#endif
	QompPlayer::stop();

	player_->stop();

	updatePlayerPosition();
}

qint64 QompQtMultimediaPlayer::currentTuneTotalTime() const
{
	if(!currentTune() || currentTune()->length == 0)
		return player_->duration();

	return currentTune()->length;
}

QStringList QompQtMultimediaPlayer::audioOutputDevice() const
{
	QStringList list;

	foreach(const QAudioDevice& dev, QMediaDevices::audioOutputs()) {
		list.append(dev.description());
	}

	return list;
}

void QompQtMultimediaPlayer::setAudioOutputDevice(const QString &devName)
{
	bool f = false;
	foreach(const QAudioDevice& dev, QMediaDevices::audioOutputs()) {
		if(dev.description() == devName) {
			player_->audioOutput()->setDevice(dev);
			f = true;
			break;
		}
	}

	if(!f) {
		player_->audioOutput()->setDevice(QMediaDevices::defaultAudioOutput());
	}

	qApp->processEvents();
}

QString QompQtMultimediaPlayer::defaultAudioDevice() const
{
	auto dev = QMediaDevices::defaultAudioOutput();
	return dev.description();
}


void QompQtMultimediaPlayer::volumeChanged(int vol)
{
	emit QompPlayer::volumeChanged(qreal(vol)/100);
}

void QompQtMultimediaPlayer::playerStateChanged(QMediaPlayer::PlaybackState _state)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::stateChanged()  " << _state;
#endif
//	if(state == QMediaPlayer::StoppedState && position() == currentTuneTotalTime())
//		emit mediaFinished();

	emit stateChanged(convertState(_state));
}

void QompQtMultimediaPlayer::updatePlayerPosition()
{
	if(currentTune() && currentTune()->length > 0) {
		const qint64 pos = currentTune()->start;

		const QSignalBlocker b(player_);
		player_->setPosition(pos);
		qApp->processEvents();
	}
}

void QompQtMultimediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::mediaStatusChanged()  " << status;
#endif
	switch(status) {
	case QMediaPlayer::LoadedMedia:
		emit QompPlayer::stateChanged(state());
		audioReadyChanged(player_->hasAudio());
		break;
	case QMediaPlayer::LoadingMedia:
	case QMediaPlayer::StalledMedia:
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
		// fall through
	default:
		emit QompPlayer::stateChanged(state());
		break;
	}
}

void QompQtMultimediaPlayer::setPlayerMediaContent(const QUrl &url)
{
	if(player_->source().isEmpty() || player_->source() != url) {
		if(url.isLocalFile() || url.isEmpty()) {
			player_->setSource(url);
		}
		else {
#if 0
			QNetworkRequest nr(url);
			nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

			if(url.toString().right(1) == ";") //SHOUTcast server
				nr.setRawHeader("icy-metadata", "1");

			player_->setMedia(QMediaContent(nr));
#else
			player_->setSource(url);
#endif
		}
	}

	resumePlayer();
}

bool QompQtMultimediaPlayer::isTuneChangeFinished() const
{
	return prevTune_ == currentTune();
}

void QompQtMultimediaPlayer::processMediaState(bool audioReady, bool seekable)
{
	if(isMediaReady(audioReady, seekable))
		emit mediaReady();
}

void QompQtMultimediaPlayer::resumePlayer()
{
	//we need to do this 2 times before and after state changing
	updatePlayerPosition();

	switch (lastAction()) {
	case Qomp::StatePlaying: {
		doPlay();
		break;
	}
	default: player_->stop();
		break;
	}

	tuneDurationChanged(player_->duration());
	updatePlayerPosition();
}

bool QompQtMultimediaPlayer::isMediaReady(bool audioReady, bool seekable) const
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::isMediaReady" << audioReady << seekable;
#endif
	return audioReady && seekable && player_->mediaStatus() == QMediaPlayer::LoadedMedia;
}

void QompQtMultimediaPlayer::doPlay()
{
//	if(isMediaReady(player_->isAudioAvailable(), player_->isSeekable())) {
		player_->play();
//	}
//	else {
//		auto pConn = QSharedPointer<QMetaObject::Connection>::create();
//		*pConn = connect(this, &QompPlayer::mediaReady,
//				 [this, pConn]()
//		{
//			player_->play();
//			disconnect(*pConn);
//		});
//	}
}

void QompQtMultimediaPlayer::audioReadyChanged(bool ready)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::audioReadyChanged " << ready;
#endif
	if(ready) {
		tuneDurationChanged(player_->duration());
		updatePlayerPosition();
	}

	processMediaState(ready, player_->isSeekable());
}

void QompQtMultimediaPlayer::seekableChanged(bool seekable)
{
	processMediaState(player_->hasAudio(), seekable);
}

void QompQtMultimediaPlayer::tuneUrlReady(const QUrl &url)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompQtMultimediaPlayer::tuneUrlReady()  " << lastAction() << url;
#endif
	if(!url.isEmpty() && resolver_) {
		resolver_->resolve( {qMakePair(currentTune(), url)} );
	}

	prevTune_ = currentTune();

	setPlayerMediaContent(url);
}

void QompQtMultimediaPlayer::tuneDurationChanged(qint64 dur)
{
	if(currentTune()->length == 0)
		emit currentTuneTotalTimeChanged(dur);
	else
		emit currentTuneTotalTimeChanged(currentTune()->length);
}

void QompQtMultimediaPlayer::tunePositionChanged(qint64 pos)
{
	const int curPos = mapPositionFromTrack(pos);
	if(currentTune()->length == 0 || curPos < currentTune()->length) {
		emit currentPositionChanged(curPos);
	}
	else {
		const QSignalBlocker b(player_);
		player_->stop();
		qApp->processEvents();

		emit mediaFinished();
	}
}
