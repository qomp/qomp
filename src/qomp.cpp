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
#include "options.h"
#include "defines.h"
#include "qompnetworkingfactory.h"
#include "qompplayer.h"

#include <QApplication>

Qomp::Qomp(QObject *parent) :
	QObject(parent)
{
	mainWin_ = new QompMainWin();
	connect(mainWin_, SIGNAL(exit()), SLOT(exit()));

	if(Options::instance()->getOption(OPTION_START_MINIMIZED).toBool())
		mainWin_->hide();

	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(updateOptions()));
}

Qomp::~Qomp()
{
	delete mainWin_;
}

void Qomp::init()
{
	updateOptions();
}

void Qomp::exit()
{
	qApp->exit();
}

void Qomp::updateOptions()
{
	QompNetworkingFactory::instance()->updateProxySettings();
	mainWin_->player()->setAudioOutputDevice(Options::instance()->getOption(OPTION_AUDIO_DEVICE, -1).toInt());
}
