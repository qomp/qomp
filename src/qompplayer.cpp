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
	connect(mediaObject_, SIGNAL(tick(qint64)), this, SIGNAL(currentPosition(qint64)));
	connect(mediaObject_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SIGNAL(stateChanged(Phonon::State,Phonon::State)));
	connect(mediaObject_, SIGNAL(finished()), SIGNAL(mediaFinished()));
}

void QompPlayer::setSeekSlider(Phonon::SeekSlider *slider)
{
	slider->setMediaObject(mediaObject_);
}

void QompPlayer::setVolumeSlider(Phonon::VolumeSlider *slider)
{
	slider->setAudioOutput(audioOutput_);
}

Phonon::State QompPlayer::state() const
{
	return mediaObject_->state();
}

void QompPlayer::setSource(const Phonon::MediaSource &source)
{
	mediaObject_->setCurrentSource(source);
}

Phonon::MediaSource QompPlayer::currentSource() const
{
	return mediaObject_->currentSource();
}

void QompPlayer::play()
{
	if(state() == Phonon::PlayingState)
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
