#include "qompqtmultimediaplayer.h"

#include <QMediaPlayer>
#include <QMediaContent>
#include <QAudioDeviceInfo>

QompQtMultimediaPlayer::QompQtMultimediaPlayer(QObject *parent) :
	QompPlayer(parent),
	player_(new QMediaPlayer(this))
{
	connect(player_, SIGNAL(positionChanged(qint64)), SIGNAL(currentPositionChanged(qint64)));
	connect(player_, SIGNAL(volumeChanged(int)), SLOT(volumeChanged(int)));
	connect(player_, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
	connect(player_, SIGNAL(durationChanged(qint64)), SIGNAL(currentTuneTotalTimeChanged(qint64)));
	connect(player_, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QMediaPlayer::State)));
	connect(player_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
}

void QompQtMultimediaPlayer::setTune(const Tune &tune)
{
	QompPlayer::setTune(tune);
	if(!tune.file.isEmpty())
		player_->setMedia(QMediaContent(tune.file));
	else
		player_->setMedia(QMediaContent(tune.url));
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
		return QompPlayer::StatePlaing;
	case QMediaPlayer::PausedState:
		return QompPlayer::StatePaused;
	}
	return QompPlayer::StateUnknown;
}

void QompQtMultimediaPlayer::playOrPause()
{
	if(state() == QompPlayer::StatePlaing)
		player_->pause();
	else
		player_->play();
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

