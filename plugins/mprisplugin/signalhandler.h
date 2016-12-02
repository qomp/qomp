/*
 * Copyright (C) 2016  Khryukin Evgeny, Vitaly Tonkacheyev
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

#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <QObject>

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

class SignalHandler : public QObject
{
	Q_OBJECT
public:
	explicit SignalHandler(QObject *parent = 0);
	static SignalHandler *instance();
	void emitSignal(SignalType type, double userValue = 0);

signals:
	void dataChanged(SignalType type);
	void volumeChanged(const double &value);

private:
	static SignalHandler *instance_;
};

#endif // SIGNALHANDLER_H
