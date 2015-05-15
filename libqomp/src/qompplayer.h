/*
 * Copyright (C) 2013-2015  Khryukin Evgeny
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

#include <QObject>

#include "common.h"
#include "libqomp_global.h"

class QompMetaDataResolver;
class Tune;
class QompPlayerImpl;

class LIBQOMPSHARED_EXPORT QompPlayer : public QObject
{
	Q_OBJECT
public:
	explicit QompPlayer(QompPlayerImpl* impl);
	~QompPlayer();

	Tune* currentTune() const;

	void resolveMetadata(const QList<Tune*> &tunes);
	Qomp::State lastAction() const;

	virtual void setVolume(qreal vol);
	/**
	 * Volume from 0 to 1
	 **/
	virtual qreal volume() const;
	virtual void setMute(bool mute);
	virtual bool isMuted() const;
	virtual void setPosition(qint64 pos);
	/**
	 * Position in miliseconds
	 */
	virtual qint64 position() const;

	virtual Qomp::State state() const;
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual qint64 currentTuneTotalTime() const;

	virtual QStringList audioOutputDevice() const;
	virtual void setAudioOutputDevice(const QString& devName);

public slots:
	void setTune(Tune* tune);
	
signals:
	void currentPositionChanged(qint64 pos);
	void currentTuneTotalTimeChanged(qint64 time);
	void stateChanged(Qomp::State newState);
	void mediaFinished();
	void volumeChanged(qreal newVolume);
	void mutedChanged(bool muted);
	void tuneChanged(Tune*);

	void tuneDataUpdated(Tune*);

private:
	class Private;
	Private* d;
	friend class Private;
};

#endif // QOPMPLAYER_H
