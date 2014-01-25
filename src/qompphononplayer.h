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

#ifndef QOMPPHONONPLAYER_H
#define QOMPPHONONPLAYER_H

#include "qompplayer.h"
#include <Phonon/MediaObject>

namespace Phonon {
class AudioOutput;
}
class QompMetaDataResolver;

class QompPhononPlayer : public QompPlayer
{
	Q_OBJECT
public:
	explicit QompPhononPlayer(QObject *parent);
	~QompPhononPlayer();

	virtual void setVolume(qreal vol);
	virtual qreal volume() const;
	virtual void setMute(bool mute);
	virtual bool isMuted() const;
	virtual void setPosition(qint64 pos);
	virtual qint64 position() const;

	virtual State state() const;
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual qint64 currentTuneTotalTime() const;

	virtual QStringList audioOutputDevice() const;
	virtual void setAudioOutputDevice(const QString& newDev);

protected:
	virtual void doSetTune(const Tune &tune);
	virtual QompMetaDataResolver* metaDataResolver() const { return resolver_; }

private slots:
	void stateChanged();

private:
	Phonon::MediaObject* mediaObject_;
	Phonon::AudioOutput* audioOutput_;
	Phonon::AudioOutputDevice defaultDevice_;
	QompMetaDataResolver* resolver_;
};

#endif // QOMPPHONONPLAYER_H
