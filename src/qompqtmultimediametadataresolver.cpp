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

#include "qompqtmultimediametadataresolver.h"

QompQtMultimediaMetaDataResolver::QompQtMultimediaMetaDataResolver(QObject *parent) :
	QompMetaDataResolver(parent),
	resolver_(new QMediaPlayer())
{
	connect(resolver_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(resolverStateChanged(QMediaPlayer::MediaStatus)));
	connect(resolver_, SIGNAL(metaDataChanged(/*QString,QVariant*/)), SLOT(metaDataReady(/*QString,QVariant*/)));
	connect(resolver_, SIGNAL(durationChanged(qint64)), SLOT(totalTimeChanged(qint64)));
}

QompQtMultimediaMetaDataResolver::~QompQtMultimediaMetaDataResolver()
{
	delete resolver_;
}

void QompQtMultimediaMetaDataResolver::resolve(const TuneList &tunes)
{
	//Currently does not work
	return;

	bool start = data_.isEmpty();
	foreach(const Tune& tune, tunes) {
		data_.append(ResolvedData(tune));
	}

	if(start) {
		resolver_->setMedia(mediaForTune(data_.first().tune));
	}
}

void QompQtMultimediaMetaDataResolver::metaDataReady()
{
	if(resolver_->isMetaDataAvailable()) {
		ResolvedData& data = data_.first();
		data.metaData.insert("ARTIST", resolver_->metaData("AlbumArtist").toString());
		data.metaData.insert("ALBUM", resolver_->metaData("AlbumTitle").toString());
		data.metaData.insert("TITLE", resolver_->metaData("Title").toString());
		data.metaData.insert("TRACK-NUMBER", resolver_->metaData("TrackNumber").toString());
		data.metaData.insert("BITRATE", resolver_->metaData("AudioBitRate").toString());
	}
}

void QompQtMultimediaMetaDataResolver::resolverStateChanged(QMediaPlayer::MediaStatus newState)
{
	if(newState != QMediaPlayer::LoadedMedia)
		return;

	if(!data_.isEmpty()) {
		ResolvedData data = data_.takeFirst();
		emit newDuration(data.tune, data.duration);
		emit newMetaData(data.tune, data.metaData);
	}

	if(!data_.isEmpty()) {
		resolver_->setMedia(mediaForTune(data_.first().tune));
	}
	else
		resolver_->setMedia(QMediaContent());
}

void QompQtMultimediaMetaDataResolver::totalTimeChanged(qint64 msec)
{
	if(!data_.isEmpty()) {
		ResolvedData& data = data_.first();
		data.duration = msec;
	}
}


QMediaContent QompQtMultimediaMetaDataResolver::mediaForTune(const Tune &tune) const
{
	QMediaContent mc;
	if(!tune.file.isEmpty()) {
		mc = QMediaContent(tune.file);
	}
	else if(!tune.url.isEmpty()) {
		mc = QMediaContent(QUrl(tune.url));
	}
	return mc;
}
