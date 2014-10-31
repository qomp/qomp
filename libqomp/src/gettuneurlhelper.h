/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#ifndef GETTUNEURLHELPER_H
#define GETTUNEURLHELPER_H

#include <QObject>
#include <QFutureWatcher>

#include "libqomp_global.h"

class Tune;

class LIBQOMPSHARED_EXPORT GetTuneUrlHelper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool blocked READ blocked WRITE setBlocked)
public:
	GetTuneUrlHelper(QObject* target, const char* slot, QObject *parent = 0);
	QFutureWatcher<QUrl>* getTuneUrlAsynchronously(Tune* t);

	bool blocked() const;
	void setBlocked(bool b);

private slots:
	void urlFinished();

private:
	QObject* target_;
	const char* slot_;
	bool blocked_;
};

#endif // GETTUNEURLHELPER_H
