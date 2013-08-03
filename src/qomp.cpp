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
#include "translator.h"
#ifdef HAVE_PHONON
#include "qompphononplayer.h"
#elif HAVE_QTMULTIMEDIA
#include "qompqtmultimediaplayer.h"
#endif

#include <QApplication>

Qomp::Qomp(QObject *parent) :
	QObject(parent)
{
	Translator::instance();

	mainWin_ = new QompMainWin();
	connect(mainWin_, SIGNAL(exit()), SLOT(exit()));

#ifdef HAVE_PHONON
	mainWin_->setPlayer(new QompPhononPlayer(this));
#elif HAVE_QTMULTIMEDIA
	mainWin_->setPlayer(new QompQtMultimediaPlayer(this));
#endif

	if(Options::instance()->getOption(OPTION_START_MINIMIZED).toBool())
		mainWin_->hide();
	else
		mainWin_->show();

	if(Options::instance()->getOption(OPTION_AUTOSTART_PLAYBACK).toBool())
		mainWin_->actPlayActivated();

	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(updateOptions()));
}

Qomp::~Qomp()
{
	delete mainWin_;
}

void Qomp::init()
{
	QVariant vVer = Options::instance()->getOption(OPTION_APPLICATION_VERSION);
	if(vVer == QVariant::Invalid) { //First launch
		QHash <const char*, QVariant> hash;
		hash.insert(OPTION_START_MINIMIZED,	false);
		hash.insert(OPTION_AUTOSTART_PLAYBACK,	false);
		hash.insert(OPTION_UPDATE_METADATA,	false);
		hash.insert(OPTION_PROXY_USE,		false);
		hash.insert(OPTION_PROXY_TYPE,		"HTTP");
		hash.insert(OPTION_PROXY_PORT,		"3128");
		hash.insert(OPTION_HIDE_ON_CLOSE,	true);
		hash.insert(OPTION_DEFAULT_ENCODING,	"CP1251");
		hash.insert(OPTION_TRAY_DOUBLE_CLICK,	1);
		hash.insert(OPTION_TRAY_MIDDLE_CLICK,	0);
		hash.insert(OPTION_TRAY_LEFT_CLICK,	2);

		foreach(const char* key, hash.keys()) {
			Options::instance()->setOption(key, hash.value(key));
		}
	}
	QString ver = vVer.toString();
	if(ver != APPLICATION_VERSION) {
		//Here in the future we can do some updates
		if(Options::instance()->getOption(OPTION_REPEAT_ALL) == QVariant::Invalid) {
			Options::instance()->setOption(OPTION_REPEAT_ALL, false);
		}
		Options::instance()->setOption(OPTION_APPLICATION_VERSION, APPLICATION_VERSION);
	}
	updateOptions();
}

void Qomp::exit()
{
	qApp->exit();
}

void Qomp::updateOptions()
{
	QompNetworkingFactory::instance()->updateProxySettings();
	if(mainWin_->player())
		mainWin_->player()->setAudioOutputDevice(Options::instance()->getOption(OPTION_AUDIO_DEVICE).toString());
}
