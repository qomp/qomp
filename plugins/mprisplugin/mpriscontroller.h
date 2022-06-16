/*
 * Copyright (C) 2013  Khryukin Evgeny, Vitaly Tonkacheyev
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

#ifndef MPRISCONTROLLER_H
#define MPRISCONTROLLER_H

#include "mprisadapter.h"
#include <QObject>

class RootAdapter;

enum SignalType {
	PLAY = 0,
	PAUSE = 1,
	STOP = 2,
	NEXT = 3,
	PREVIOUS = 4,
	VOLUME = 5,
	QUIT = 6,
	RAISE = 7,
	POSITION = 8,
	SHUFFLE = 9,
	LOOPALL = 10
};

class MprisController : public QObject
{
	Q_OBJECT
public:
	explicit MprisController(QObject *parent = 0);
	~MprisController();

	void emitSignal(SignalType type, const qreal &userValue = 0);
	void sendData(const QString &status, const QompMetaData &tune);
	qreal getVolume();
	qreal getPosition();
	void setVolume(const qreal &volume);
	void setPosition(const qreal &pos);
	void setLoopAndShuffle(bool loop, bool shuffle) { mprisAdapter_->setLoopAndShuffle(loop, shuffle); };

signals:
	void play();
	void pause();
	void next();
	void previous();
	void stop();
	void volumeChanged(const qreal &volume);
	void sendQuit();
	void sendRaise();
	void updateVolume();
	void updatePosition();
	void positionChanged(const qreal &position);
	void shuffleUpdated();
	void loopStatusUpdated();

private:
	RootAdapter *rootAdapter_;
	MprisAdapter *mprisAdapter_;
	qreal volume_;
	qreal position_;
};

#endif // MPRISCONTROLLER_H
