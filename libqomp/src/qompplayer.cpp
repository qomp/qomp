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

#include "qompplayer.h"
#include "qompplayerimpl.h"
#include "qompmetadataresolver.h"
#include "tune.h"

#include <QThread>


class QompPlayer::Private
{
public:
	explicit Private(QompPlayer* p) : _player(p){}

	void connectImpl()
	{
		connect(impl, SIGNAL(currentPositionChanged(qint64)), _player, SIGNAL(currentPositionChanged(qint64)), Qt::QueuedConnection);
		connect(impl, SIGNAL(currentTuneTotalTimeChanged(qint64)), _player, SIGNAL(currentTuneTotalTimeChanged(qint64)), Qt::QueuedConnection);
		connect(impl, SIGNAL(stateChanged(Qomp::State)), _player, SIGNAL(stateChanged(Qomp::State)), Qt::QueuedConnection);
		connect(impl, SIGNAL(mediaFinished()), _player, SIGNAL(mediaFinished()), Qt::QueuedConnection);
		connect(impl, SIGNAL(volumeChanged(qreal)), _player, SIGNAL(volumeChanged(qreal)), Qt::QueuedConnection);
		connect(impl, SIGNAL(mutedChanged(bool)), _player, SIGNAL(mutedChanged(bool)), Qt::QueuedConnection);
		connect(impl, SIGNAL(tuneChanged(Tune*)), _player, SIGNAL(tuneChanged(Tune*)), Qt::QueuedConnection);
		connect(impl, SIGNAL(tuneDataUpdated(Tune*)), _player, SIGNAL(tuneDataUpdated(Tune*)), Qt::QueuedConnection);
	}

public:
	QompPlayer* _player;
	QompPlayerImpl* impl;
	QThread thread;
};



QompPlayer::QompPlayer(QompPlayerImpl* impl) :
	QObject(),
	d(new Private(this))
{
	Q_ASSERT(impl != nullptr);

	d->impl = impl;	
	d->impl->moveToThread(&d->thread);
	d->connectImpl();
	d->thread.start();
}

QompPlayer::~QompPlayer()
{
	d->thread.quit();
	delete d->impl;
	delete d;
}

void QompPlayer::setTune(Tune *tune)
{
	QMetaObject::invokeMethod(d->impl, "setTune", Qt::QueuedConnection, Q_ARG(Tune*, tune));
}

Tune *QompPlayer::currentTune() const
{
	Tune* r;
	QMetaObject::invokeMethod(d->impl, "currentTune", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(Tune*, r));
	return r;
}

void QompPlayer::resolveMetadata(const QList<Tune*>& tunes)
{
	QMetaObject::invokeMethod(d->impl, "resolveMetadata", Qt::QueuedConnection, Q_ARG(QList<Tune*>, tunes));
}

Qomp::State QompPlayer::lastAction() const
{
	Qomp::State r;
	QMetaObject::invokeMethod(d->impl, "lastAction", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(Qomp::State, r));
	return r;
}

void QompPlayer::setVolume(qreal vol)
{
	QMetaObject::invokeMethod(d->impl, "setVolume", Qt::QueuedConnection, Q_ARG(qreal, vol));
}

qreal QompPlayer::volume() const
{
	qreal r;
	QMetaObject::invokeMethod(d->impl, "volume", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(qreal, r));
	return r;
}

void QompPlayer::setMute(bool mute)
{
	QMetaObject::invokeMethod(d->impl, "setMute", Qt::QueuedConnection, Q_ARG(bool, mute));
}

bool QompPlayer::isMuted() const
{
	bool r;
	QMetaObject::invokeMethod(d->impl, "isMuted", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(bool, r));
	return r;
}

void QompPlayer::setPosition(qint64 pos)
{
	QMetaObject::invokeMethod(d->impl, "setPosition", Qt::QueuedConnection, Q_ARG(qint64, pos));
}

qint64 QompPlayer::position() const
{
	qint64 r;
	QMetaObject::invokeMethod(d->impl, "position", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(qint64, r));
	return r;
}

Qomp::State QompPlayer::state() const
{
	Qomp::State r;
	QMetaObject::invokeMethod(d->impl, "state", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(Qomp::State, r));
	return r;
}

void QompPlayer::play()
{
	QMetaObject::invokeMethod(d->impl, "play", Qt::QueuedConnection);
}

void QompPlayer::pause()
{
	QMetaObject::invokeMethod(d->impl, "pause", Qt::QueuedConnection);
}

void QompPlayer::stop()
{
	QMetaObject::invokeMethod(d->impl, "stop", Qt::QueuedConnection);
}

qint64 QompPlayer::currentTuneTotalTime() const
{
	qint64 r;
	QMetaObject::invokeMethod(d->impl, "currentTuneTotalTime", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(qint64, r));
	return r;
}

QStringList QompPlayer::audioOutputDevice() const
{
	QStringList r;
	QMetaObject::invokeMethod(d->impl, "audioOutputDevice", Qt::BlockingQueuedConnection,
				  Q_RETURN_ARG(QStringList, r));
	return r;
}

void QompPlayer::setAudioOutputDevice(const QString &devName)
{
	QMetaObject::invokeMethod(d->impl, "setAudioOutputDevice", Qt::QueuedConnection, Q_ARG(QString, devName));
}
