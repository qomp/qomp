#ifndef QOPMPLAYER_H
#define QOPMPLAYER_H

#include <QObject>

namespace Phonon {
	class SeekSlider;
	class VolumeSlider;
	class AudioOutput;
	class MediaSource;
}

#include <Phonon/MediaObject>

class QompPlayer : public QObject
{
	Q_OBJECT
public:
	QompPlayer(QObject *parent = 0);
	void setSeekSlider(Phonon::SeekSlider* slider);
	void setVolumeSlider(Phonon::VolumeSlider* slider);
	Phonon::State state() const;
	void setSource(const Phonon::MediaSource& source);
	Phonon::MediaSource currentSource() const;
	void play();
	void stop();
	
signals:
	void currentPosition(qint64 pos);
	void stateChanged(Phonon::State newState, Phonon::State oldState);
	void mediaFinished();
	
public slots:




private:
	Phonon::MediaObject* mediaObject_;
	Phonon::AudioOutput* audioOutput_;
	
};

#endif // QOPMPLAYER_H
