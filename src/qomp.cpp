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

#include "qomp.h"
#include "qompmainwin.h"

#include <QEvent>
#include <QApplication>
#include <QtPlugin>


Qomp::Qomp(QObject *parent) :
	QObject(parent)
{
	mainWin_ = new QompMainWin();
	mainWin_->installEventFilter(this);

	mainWin_->show();
}

Qomp::~Qomp()
{
	delete mainWin_;
}

void Qomp::init()
{
}

bool Qomp::eventFilter(QObject *obj, QEvent *e)
{
	if(obj == mainWin_) {
		if(e->type() == QEvent::Close) {
			e->accept();
			qApp->exit();
			return true;
		}
	}

	return QObject::eventFilter(obj, e);
}
