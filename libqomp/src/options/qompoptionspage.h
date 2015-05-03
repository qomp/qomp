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

#ifndef QOMPOPTIONSPAGE_H
#define QOMPOPTIONSPAGE_H

#include <QObject>
#include "libqomp_global.h"

class QompPlayer;

class LIBQOMPSHARED_EXPORT QompOptionsPage : public QObject
{
	Q_OBJECT
public:
	explicit QompOptionsPage(QObject *parent = 0) : QObject(parent) {}
	virtual QString name() const = 0;
	virtual void retranslate() = 0;
	virtual void init(QompPlayer* player) { Q_UNUSED(player) }
	virtual QObject* page() const = 0;

public slots:
	virtual void applyOptions() = 0;
	virtual void restoreOptions() = 0;
};

#endif // QOMPOPTIONSPAGE_H
