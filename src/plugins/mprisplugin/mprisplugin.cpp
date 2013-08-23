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
#include "mpris.h"
#include "qompplayer.h"

#include <QTimer>
#include <QtPlugin>

MprisPlugin::MprisPlugin() :
	player_(0),
	enabled_(true),
	mpris_(0)
{
}

void MprisPlugin::qompPlayerChanged(QompPlayer *player)
{
	player_ = player;
	connect(player_, SIGNAL(stateChanged(QompPlayer::State)), SLOT(playerStatusChanged()));
}

void MprisPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
	if(enabled_)
		connectToDbus();
	else
		disconnectFromDbus();
}

void MprisPlugin::unload()
{
	disconnectFromDbus();
}

void MprisPlugin::playerStatusChanged()
{
	if(!enabled_ || !mpris_)
		return;

	const Tune& t = player_->currentTune();
	int num = t.trackNumber.isEmpty() ? 0 : t.trackNumber.toInt();
	switch(player_->state()) {
	case QompPlayer::StatePlaying:
		mpris_->setMetadata(num, t.title, t.artist, t.album, t.url);
		mpris_->setStatus("Playing");
		mpris_->sendProperties();
		break;
	case QompPlayer::StateStopped:
		mpris_->setMetadata(num, t.title, t.artist, t.album, t.url);
		mpris_->setStatus("Stopped");
		mpris_->sendProperties();
		break;
	default:
		break;
	}
}

void MprisPlugin::connectToDbus()
{
	mpris_ = new Mpris(this);
}

void MprisPlugin::disconnectFromDbus()
{
	delete mpris_;
	mpris_ = 0;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(mprisplugin, MprisPlugin)
#endif
