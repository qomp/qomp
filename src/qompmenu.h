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

#ifndef QOMPMENU_H
#define QOMPMENU_H

#include <QMenu>

#include "tune.h"

class QompGetTunesMenu : public QMenu
{
	Q_OBJECT
public:
	explicit QompGetTunesMenu(QWidget *parent = 0);
	QompGetTunesMenu(const QString& name, QWidget *parent = 0);

signals:
	void tunes(const TuneList&);

private slots:
	void actionActivated();

private:
	void buildMenu();
};

#endif // QOMPMENU_H
