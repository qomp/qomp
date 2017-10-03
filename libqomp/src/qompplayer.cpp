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

#include "qompplayer.h"
#include "qompmetadataresolver.h"
#include "tune.h"
#include "covercache.h"


QompPlayer::QompPlayer() :
	QObject(),
	currentTune_(nullptr),
	lastAction_(Qomp::StateStopped)
{
	CoverCache::instance(); //Init Cover Cache;
}

void QompPlayer::setTune(Tune *tune)
{
	currentTune_ = tune;
	QMetaObject::invokeMethod(this, "doSetTune", Qt::QueuedConnection);
	emit tuneChanged(tune);
}

Tune *QompPlayer::currentTune() const
{
	return currentTune_;
}

//void QompPlayer::resolveMetadata(const QList<Tune*>& tunes)
//{
//	if(metaDataResolver())
//		metaDataResolver()->resolve(tunes);
//}

Qomp::State QompPlayer::lastAction() const
{
	return lastAction_;
}

void QompPlayer::play()
{
	lastAction_ = Qomp::StatePlaying;
}

void QompPlayer::pause()
{
	lastAction_ = Qomp::StatePaused;
}

void QompPlayer::stop()
{
	lastAction_ = Qomp::StateStopped;
}
