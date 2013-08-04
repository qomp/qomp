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

#ifndef QOPMPLAYER_H
#define QOPMPLAYER_H

#include "tune.h"
#include <QObject>

class QompMetaDataResolver;

class QompPlayer : public QObject
{
	Q_OBJECT
public:
	QompPlayer(QObject *parent = 0);

	enum State { StateUnknown = 0,
		StateStopped, StatePaused,
		StatePlaying, StateError,
		StateLoading, StateBuffering
	};

	void setTune(const Tune& tune);
	const Tune& currentTune() const;

	virtual QompMetaDataResolver* metaDataResolver() const { return 0; }

	virtual void setVolume(qreal vol) = 0;
	/**
	 * Volume from 0 to 1
	 **/
	virtual qreal volume() const = 0;
	virtual void setMute(bool mute) = 0;
	virtual bool isMuted() const = 0;
	virtual void setPosition(qint64 pos) = 0;
	/**
	 * Position in miliseconds
	 */
	virtual qint64 position() const = 0;

	virtual State state() const = 0;
	virtual void play() = 0;
	virtual void pause() = 0;
	virtual void stop() = 0;
	virtual qint64 currentTuneTotalTime() const = 0;

	virtual QStringList audioOutputDevice() const = 0;
	virtual void setAudioOutputDevice(const QString& devName) = 0;
	
signals:
	void currentPositionChanged(qint64 pos);
	void currentTuneTotalTimeChanged(qint64 time);
	void stateChanged(QompPlayer::State newState);
	void mediaFinished();
	void volumeChanged(qreal newVolume);
	void mutedChanged(bool muted);
	void tuneChanged(const Tune&);

protected:
	virtual void doSetTune(const Tune& tune) = 0;

private:
	Tune currentTune_;
};

#endif // QOPMPLAYER_H
