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

#include "mprisplugin.h"
#include "qompplayer.h"
#include "tune.h"

#include <QTimer>
#include <QtPlugin>

#define STOPPED "Stopped"
#define PAUSED "Paused"
#define PLAYING "Playing"

MprisPlugin::MprisPlugin() :
	player_(0),
	enabled_(true),
	mpris_(0),
	tune_(0)
{
}

void MprisPlugin::qompPlayerChanged(QompPlayer *player)
{
	player_ = player;
	connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStatusChanged()));
	connect(player_, SIGNAL(tuneDataUpdated(Tune*)), SLOT(tuneInfoLoaded(Tune*)));
}

void MprisPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
	if(enabled_) {
		mpris_ = new MprisController(this);
		tune_ = new QompMetaData();
	}
	else {
		disableMpris();
	}
}

void MprisPlugin::unload()
{
	disableMpris();
}

void MprisPlugin::playerStatusChanged()
{
	if(!enabled_ || !mpris_)
		return;

	switch(player_->state()) {
		case Qomp::StatePlaying:
			getMetaData();
			sendMetadata(PLAYING);
			break;
		case Qomp::StateStopped:
			sendMetadata(STOPPED);
			break;
		case Qomp::StatePaused:
			sendMetadata(PAUSED);
			break;
		default:
			break;
	}
}

void MprisPlugin::tuneInfoLoaded(Tune *tuneInfo)
{
	Q_UNUSED(tuneInfo);
	if(!enabled_ || !mpris_)
		return;

	if (player_->state() == Qomp::StatePlaying) {
		getMetaData();
		sendMetadata(PLAYING);
	}
}

void MprisPlugin::getMetaData()
{
	Tune* t = player_->currentTune();
	if (t) {
		int num = t->trackNumber.isEmpty() ? 0 : t->trackNumber.toInt();
		tune_->artist = t->artist;
		tune_->title= t->title;
		tune_->album = t->album;
		tune_->trackNumber = num;
		tune_->url = t->getUrl().toString();
	}
}

void MprisPlugin::sendMetadata(const QString &status)
{
	if (status == STOPPED || status == PAUSED) {
		mpris_->sendData(status, QompMetaData());
	}
	else if (status == PLAYING){
		mpris_->sendData(status, *tune_);
	}
}

void MprisPlugin::disableMpris()
{
	delete mpris_;
	mpris_ = 0;
	delete tune_;
	tune_ = 0;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(mprisplugin, MprisPlugin)
#endif
