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

#include "qompphononplayer.h"
#include "qompphononmetadataresolver.h"

#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>
#include <phonon/BackendCapabilities>

//#define DEBUG_OUTPUT

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

QompPhononPlayer::QompPhononPlayer(QObject *parent) :
	QompPlayer(parent),
	resolver_(new QompPhononMetaDataResolver(this))
{
	mediaObject_ = new Phonon::MediaObject(this);
	audioOutput_ = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	/*Phonon::Path path = */Phonon::createPath(mediaObject_, audioOutput_);
	defaultDevice_ = audioOutput_->outputDevice();

	mediaObject_->setTickInterval(1000);
	connect(mediaObject_, SIGNAL(tick(qint64)), SIGNAL(currentPositionChanged(qint64)));
	connect(mediaObject_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(stateChanged()));
	connect(mediaObject_, SIGNAL(finished()), SIGNAL(mediaFinished()));
	connect(mediaObject_, SIGNAL(totalTimeChanged(qint64)), SIGNAL(currentTuneTotalTimeChanged(qint64)));

	connect(audioOutput_, SIGNAL(volumeChanged(qreal)), SIGNAL(volumeChanged(qreal)));
	connect(audioOutput_, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
}

QompPhononPlayer::~QompPhononPlayer()
{
	stop();
}

void QompPhononPlayer::setVolume(qreal vol)
{
	audioOutput_->setVolume(vol);
}

qreal QompPhononPlayer::volume() const
{
	return audioOutput_->volume();
}

void QompPhononPlayer::setMute(bool mute)
{
	audioOutput_->setMuted(mute);
}

bool QompPhononPlayer::isMuted() const
{
	return audioOutput_->isMuted();
}

void QompPhononPlayer::setPosition(qint64 pos)
{
	mediaObject_->seek(pos);
}

qint64 QompPhononPlayer::position() const
{
	return mediaObject_->currentTime();
}

QompPlayer::State QompPhononPlayer::state() const
{
	switch (mediaObject_->state()) {
	case Phonon::PausedState:
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPlayer::state()" << "PausedState";
#endif
		return StatePaused;
	case Phonon::PlayingState:
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPlayer::state()" << "Phonon::PlayingState";
#endif
		return StatePlaing;
	case Phonon::StoppedState:
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPlayer::state()" << "Phonon::StoppedState";
#endif
		return StateStopped;
	case Phonon::ErrorState:
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPlayer::state()" << "Phonon::ErrorState"
			    << mediaObject_->errorType() << mediaObject_->errorString();
#endif
		return StateError;
	case Phonon::BufferingState:
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPlayer::state()" << "Phonon::BufferingState";
#endif
		return StateBuffering;
	case Phonon::LoadingState:
#ifdef DEBUG_OUTPUT
		qDebug() << "QompPlayer::state()" << "Phonon::LoadingState";
#endif
		return StateLoading;
	}
	return StateUnknown;
}

void QompPhononPlayer::setTune(const Tune &tune)
{
	QompPlayer::setTune(tune);

	Phonon::MediaSource ms;
	if(!tune.file.isEmpty()) {
		ms = Phonon::MediaSource(tune.file);
	}
	else if(!tune.url.isEmpty()) {
		ms = Phonon::MediaSource(tune.url);
	}
	mediaObject_->setCurrentSource(ms);
}

void QompPhononPlayer::playOrPause()
{
	if(mediaObject_->state() == Phonon::PlayingState)
		mediaObject_->pause();
	else
		mediaObject_->play();
}

void QompPhononPlayer::stop()
{
	mediaObject_->stop();
}

qint64 QompPhononPlayer::currentTuneTotalTime() const
{
	return mediaObject_->totalTime();
}

QStringList QompPhononPlayer::audioOutputDevice() const
{
	QStringList ret;
	QList<Phonon::AudioOutputDevice> audioOutputDevices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	foreach(Phonon::AudioOutputDevice dev, audioOutputDevices) {
		ret.append(dev.name());
	}
	return ret;
}

void QompPhononPlayer::setAudioOutputDevice(const QString &newDev)
{
	if(newDev.isEmpty()) {
		audioOutput_->setOutputDevice(defaultDevice_);
		return;
	}

	QList<Phonon::AudioOutputDevice> audioOutputDevices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	foreach(Phonon::AudioOutputDevice dev, audioOutputDevices) {
		if(dev.name() == newDev) {
			audioOutput_->setOutputDevice(dev);
			break;
		}
	}
}

void QompPhononPlayer::stateChanged()
{
	emit QompPlayer::stateChanged(state());
}