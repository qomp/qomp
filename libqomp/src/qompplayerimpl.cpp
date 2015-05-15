/*
 * Copyright (C) 2015  Khryukin Evgeny
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

#include "qompplayerimpl.h"
#include "qompmetadataresolver.h"
#include "tune.h"


QompPlayerImpl::QompPlayerImpl() :
	QObject(),
	currentTune_(nullptr),
	lastAction_(Qomp::StateStopped)
{
}

void QompPlayerImpl::setTune(Tune *tune)
{
	currentTune_ = tune;
	doSetTune();
	emit tuneChanged(tune);
}

Tune *QompPlayerImpl::currentTune() const
{
	return currentTune_;
}

void QompPlayerImpl::resolveMetadata(const QList<Tune*>& tunes)
{
	if(metaDataResolver())
		metaDataResolver()->resolve(tunes);
}

Qomp::State QompPlayerImpl::lastAction() const
{
	return lastAction_;
}

void QompPlayerImpl::play()
{
	lastAction_ = Qomp::StatePlaying;
}

void QompPlayerImpl::pause()
{
	lastAction_ = Qomp::StatePaused;
}

void QompPlayerImpl::stop()
{
	lastAction_ = Qomp::StateStopped;
}

