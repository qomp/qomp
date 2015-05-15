/*
 * Copyright (C) 2015  Khryukin Evgeny
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

#ifndef QOMPPLAYERIMPL_H
#define QOMPPLAYERIMPL_H

#include <QObject>
#include <QStringList>

#include "common.h"
#include "libqomp_global.h"

class QompMetaDataResolver;
class Tune;

class LIBQOMPSHARED_EXPORT QompPlayerImpl : public QObject
{
	Q_OBJECT
public:
	Q_INVOKABLE Tune* currentTune() const;
	Q_INVOKABLE Qomp::State lastAction() const;
	/**
	 * Volume from 0 to 1
	 **/
	Q_INVOKABLE virtual qreal volume() const = 0;

	Q_INVOKABLE virtual bool isMuted() const = 0;
	/**
	 * Position in miliseconds
	 */
	Q_INVOKABLE virtual qint64 position() const = 0;

	Q_INVOKABLE virtual Qomp::State state() const = 0;
	Q_INVOKABLE virtual qint64 currentTuneTotalTime() const = 0;
	Q_INVOKABLE virtual QStringList audioOutputDevice() const = 0;

public slots:
	void resolveMetadata(const QList<Tune*> &tunes);
	void setTune(Tune* tune);

	virtual void play();
	virtual void pause();
	virtual void stop();

	virtual void setMute(bool mute) = 0;
	virtual void setVolume(qreal vol) = 0;
	virtual void setPosition(qint64 pos) = 0;

	virtual void setAudioOutputDevice(const QString& devName) = 0;

protected slots:
	virtual void doSetTune() = 0;

signals:
	void currentPositionChanged(qint64 pos);
	void currentTuneTotalTimeChanged(qint64 time);
	void stateChanged(Qomp::State newState);
	void mediaFinished();
	void volumeChanged(qreal newVolume);
	void mutedChanged(bool muted);
	void tuneChanged(Tune*);

	void tuneDataUpdated(Tune*);

protected:
	QompPlayerImpl();

	Q_INVOKABLE virtual QompMetaDataResolver* metaDataResolver() const { return 0; }

private:
	Tune* currentTune_;
	Qomp::State lastAction_;
};

#endif // QOMPPLAYERIMPL_H
