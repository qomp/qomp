#include "qompplayer.h"

#include <Phonon/AudioOutput>
#include <Phonon/SeekSlider>
#include <Phonon/VolumeSlider>

QompPlayer::QompPlayer(QObject *parent) :
	QObject(parent)
{
	mediaObject_ = new Phonon::MediaObject(this);
	audioOutput_ = new Phonon::AudioOutput(Phonon::MusicCategory, this);
	/*Phonon::Path path = */Phonon::createPath(mediaObject_, audioOutput_);

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

