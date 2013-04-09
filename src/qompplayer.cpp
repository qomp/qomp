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

#include "qompplayer.h"

#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>
#include <phonon/BackendCapabilities>

QompPlayer::QompPlayer(QObject *parent) :
	QObject(parent)
{
	mediaObject_ = new Phonon::MediaObject(this);
	audioOutput_ = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	/*Phonon::Path path = */Phonon::createPath(mediaObject_, audioOutput_);
	defaultDevice_ = audioOutput_->outputDevice();

	mediaObject_->setTickInterval(1000);
	connect(mediaObject_, SIGNAL(tick(qint64)), SIGNAL(currentPosition(qint64)));
	connect(mediaObject_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(stateChanged()));
	connect(mediaObject_, SIGNAL(finished()), SIGNAL(mediaFinished()));
}

QompPlayer::~QompPlayer()
{
	stop();
}

void QompPlayer::setSeekSlider(Phonon::SeekSlider *slider)
{
	slider->setMediaObject(mediaObject_);
}

void QompPlayer::setVolumeSlider(Phonon::VolumeSlider *slider)
{
	slider->setAudioOutput(audioOutput_);
}

QompPlayer::State QompPlayer::state() const
{
	switch (mediaObject_->state()) {
	case Phonon::PausedState:
		return StatePaused;
	case Phonon::PlayingState:
		return StatePlaing;
	case Phonon::StoppedState:
		return StateStopped;
	case Phonon::ErrorState:
		return StateError;
	case Phonon::BufferingState:
	case Phonon::LoadingState:
		return StateLoading;
	}
	return StateUnknown;
}

void QompPlayer::setTune(const Tune &tune)
{
	currentTune_ = tune;
	Phonon::MediaSource ms;
	if(!tune.file.isEmpty()) {
		ms = Phonon::MediaSource(tune.file);
	}
	else if(!tune.url.isEmpty()) {
		ms = Phonon::MediaSource(tune.url);
	}
	mediaObject_->setCurrentSource(ms);
}

Tune QompPlayer::currentTune() const
{
	return currentTune_;
}

void QompPlayer::playOrPause()
{
	if(mediaObject_->state() == Phonon::PlayingState)
		mediaObject_->pause();
	else
		mediaObject_->play();
}

void QompPlayer::stop()
{
	mediaObject_->stop();
}

void QompPlayer::setAudioOutputDevice(int index)
{
	if(index == -1) {
		audioOutput_->setOutputDevice(defaultDevice_);
		return;
	}

	QList<Phonon::AudioOutputDevice> audioOutputDevices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	foreach(Phonon::AudioOutputDevice dev, audioOutputDevices) {
		if(dev.index() == index) {
			audioOutput_->setOutputDevice(dev);
			break;
		}
	}
}


void QompPlayer::stateChanged()
{
	emit stateChanged(state());
}
