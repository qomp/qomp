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

#ifndef QOMPQTMULTIMEDIAPLAYER_H
#define QOMPQTMULTIMEDIAPLAYER_H

#include "qompplayer.h"

#include <QMediaPlayer>
class QompMetaDataResolver;

class QompQtMultimediaPlayer : public QompPlayer
{
	Q_OBJECT
public:
	explicit QompQtMultimediaPlayer(QObject *parent = 0);
	~QompQtMultimediaPlayer();

	virtual QompMetaDataResolver* metaDataResolver() const;

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

protected:
	virtual void doSetTune(const Tune &tune);

private:
	QMediaPlayer* player_;
	QompMetaDataResolver* resolver_;
	
};

#endif // QOMPQTMULTIMEDIAPLAYER_H
