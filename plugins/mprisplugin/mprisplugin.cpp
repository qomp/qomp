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
	tune_(0),
	lastTune_(0)
{
}

void MprisPlugin::qompPlayerChanged(QompPlayer *player)
{
	if(player_ != player) {
		if(player_) {
			player_->disconnect(SIGNAL(stateChanged(Qomp::State)), this, SLOT(playerStatusChanged(Qomp::State)));
		}

		player_ = player;
		if(player_) {
			connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStatusChanged(Qomp::State)));
		}
	}
}

void MprisPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
	if(enabled_) {
		mpris_ = new MprisController(this);
		tune_ = new QompMetaData();
		connect(mpris_, SIGNAL(play()), SLOT(play()));
		connect(mpris_, SIGNAL(pause()), SLOT(pause()));
		connect(mpris_, SIGNAL(stop()), SLOT(stop()));
		connect(mpris_, SIGNAL(next()), SLOT(next()));
		connect(mpris_, SIGNAL(previous()), SLOT(previous()));
		connect(mpris_, SIGNAL(volumeChanged(double)), SLOT(setVolume(double)));
	}
	else {
		disconnect(mpris_);
		disableMpris();
	}
}

void MprisPlugin::play()
{
	if(player_) {
		player_->play();
	}
}

void MprisPlugin::pause()
{
	if(player_) {
		player_->pause();
	}
}

void MprisPlugin::stop()
{
	if(player_) {
		player_->stop();
	}
}

void MprisPlugin::next()
{
	if(player_) {
		emit player_->mediaFinished(); //Temporary hack, FIXME
	}
}

void MprisPlugin::previous()
{
	if(player_) {
		//player_->previous();  //FIXME
	}
}

void MprisPlugin::setVolume(const double &volume)
{
	if(player_) {
		player_->setVolume(volume);
		emit player_->volumeChanged(volume); //Temporary hack, FIXME
	}
}

void MprisPlugin::unload()
{
	disableMpris();
}

void MprisPlugin::playerStatusChanged(Qomp::State state)
{
	if(!enabled_ || !mpris_ || !player_)
		return;

	switch(state) {
		case Qomp::StatePlaying:
			getMetaData(player_->currentTune());
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

void MprisPlugin::getMetaData(Tune *tune)
{
	if (tune && lastTune_ != tune) {
		lastTune_ = tune;
		const int num = tune->trackNumber.isEmpty() ? 0 : tune->trackNumber.toInt();
		tune_->artist = tune->artist;
		tune_->title= tune->title;
		tune_->album = tune->album;
		tune_->trackNumber = num;
		tune_->url = "";// tune->getUrl().toString();
	}
}

void MprisPlugin::sendMetadata(const QString &status)
{
	if (status == STOPPED || status == PAUSED) {
		mpris_->sendData(status, QompMetaData(), player_->volume());
	}
	else if (status == PLAYING){
		mpris_->sendData(status, *tune_, player_->volume());
	}
}

void MprisPlugin::disableMpris()
{
	delete mpris_;
	mpris_ = 0;
	delete tune_;
	tune_ = 0;
	lastTune_ = 0;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(mprisplugin, MprisPlugin)
#endif
