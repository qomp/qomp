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

#include "qompphononmetadataresolver.h"


QompPhononMetaDataResolver::QompPhononMetaDataResolver(QObject *parent) :
	QompMetaDataResolver(parent),
	resolver_(new Phonon::MediaObject(this))
{
	connect(resolver_, SIGNAL(metaDataChanged()), SLOT(metaDataReady()));
	connect(resolver_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(resolverStateChanged(Phonon::State,Phonon::State)));
	connect(resolver_, SIGNAL(totalTimeChanged(qint64)), SLOT(totalTimeChanged(qint64)));
}

QompPhononMetaDataResolver::~QompPhononMetaDataResolver()
{
	resolver_->deleteLater();
}

void QompPhononMetaDataResolver::resolve(const TuneList &tunes)
{
	bool start = data_.isEmpty();
	foreach(const Tune& tune, tunes) {
		data_.append(ResolvedData(tune));
	}

	if(start) {
		resolver_->setCurrentSource(objectForTune(data_.first().tune));
		resolver_->pause();
	}
}

void QompPhononMetaDataResolver::resolverStateChanged(Phonon::State newState, Phonon::State /*oldState*/)
{
	if(newState != Phonon::PausedState)
		return;

	if(!data_.isEmpty()) {
		ResolvedData data = data_.takeFirst();
		emit newDuration(data.tune, data.duration);
		emit newMetaData(data.tune, data.metaData);
	}

	if(!data_.isEmpty()) {
		resolver_->setCurrentSource(objectForTune(data_.first().tune));
		resolver_->pause();
	}
	else
		resolver_->clear();
}

void QompPhononMetaDataResolver::totalTimeChanged(qint64 msec)
{
	if(!data_.isEmpty()) {
		ResolvedData& data = data_.first();
		data.duration = msec;
	}
}

void QompPhononMetaDataResolver::metaDataReady()
{
	if(!data_.isEmpty()) {
		ResolvedData& data = data_.first();
		data.metaData = resolver_->metaData();
	}
}

Phonon::MediaSource QompPhononMetaDataResolver::objectForTune(const Tune &tune) const
{
	Phonon::MediaSource ms;
	ms = Phonon::MediaSource(tune.getUrl());
	return ms;
}
