#ifndef QOMPQTMULTIMEDIAPLAYER_H
#define QOMPQTMULTIMEDIAPLAYER_H

#include "qompplayer.h"

#include <QMediaPlayer>

class QompQtMultimediaPlayer : public QompPlayer
{
	Q_OBJECT
public:
	explicit QompQtMultimediaPlayer(QObject *parent = 0);
	virtual void setTune(const Tune& tune);

	virtual QompMetaDataResolver* metaDataResolver() const { return 0; }

	virtual void setVolume(qreal vol);
	virtual qreal volume() const;
	virtual void setMute(bool mute);
	virtual bool isMuted() const;
	virtual void setPosition(qint64 pos);
	virtual qint64 position() const;

	virtual State state() const;
	virtual void playOrPause();
	virtual void stop();
	virtual qint64 currentTuneTotalTime() const;

	virtual QStringList audioOutputDevice() const;
	virtual void setAudioOutputDevice(const QString& devName);

private slots:
	void volumeChanged(int);
	void stateChanged(QMediaPlayer::State);
	void mediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
	QMediaPlayer* player_;
	
};

#endif // QOMPQTMULTIMEDIAPLAYER_H
