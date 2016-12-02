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
	RAISE = 7
};

class MprisController : public QObject
{
	Q_OBJECT
public:
	explicit MprisController(QObject *parent = 0);
	~MprisController();

	void emitSignal(SignalType type, double userValue = 0);
	void sendData(const QString &status, const QompMetaData &tune, const double &volume);

signals:
	void play();
	void pause();
	void next();
	void previous();
	void stop();
	void volumeChanged(const double &volume);
	void sendQuit();
	void sendRaise();

private:
	RootAdapter *rootAdapter_;
	MprisAdapter *mprisAdapter_;
};

#endif // MPRISCONTROLLER_H
