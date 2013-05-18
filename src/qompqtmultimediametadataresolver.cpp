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
//#include "qompnetworkingfactory.h"
//#include <QNetworkAccessManager>
//#include <QNetworkCookieJar>
//#include <QNetworkCookie>

//#define DEBUG_OUTPUT

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

QompQtMultimediaMetaDataResolver::QompQtMultimediaMetaDataResolver(QObject *parent) :
	QompMetaDataResolver(parent),
	resolver_(new QMediaPlayer(this))
{
	connect(resolver_, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(resolverStateChanged(QMediaPlayer::MediaStatus)));
	connect(resolver_, SIGNAL(metaDataChanged(/*QString,QVariant*/)), SLOT(metaDataReady(/*QString,QVariant*/)));
	connect(resolver_, SIGNAL(durationChanged(qint64)), SLOT(totalTimeChanged(qint64)));
#ifdef DEBUG_OUTPUT
	connect(resolver_, SIGNAL(metaDataAvailableChanged(bool)), SLOT(metaDataAvailableChanged(bool)));
	connect(resolver_, SIGNAL(error(QMediaPlayer::Error)), SLOT(error(QMediaPlayer::Error)));
	connect(resolver_, SIGNAL(availabilityChanged(bool)), SLOT(availabilityChanged(bool)));
	connect(resolver_, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(stateChanged(QMediaPlayer::State)));
#endif
}

QompQtMultimediaMetaDataResolver::~QompQtMultimediaMetaDataResolver()
{
	delete resolver_;
	resolver_ = 0;
}

void QompQtMultimediaMetaDataResolver::resolve(const TuneList &tunes)
{
	bool start = data_.isEmpty();
	foreach(const Tune& tune, tunes) {
		data_.append(ResolvedData(tune));
	}

	if(start) {
#ifdef DEBUG_OUTPUT
		qDebug() << "Start resolving";
#endif
		resolveNextMedia();
	}
}

void QompQtMultimediaMetaDataResolver::metaDataReady()
{
#ifdef DEBUG_OUTPUT
		qDebug() << "metaDataReady";
#endif
	if(resolver_->isMetaDataAvailable()) {
#ifdef DEBUG_OUTPUT
		qDebug() << "available metadata" << resolver_->availableMetaData();
#endif
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
#ifdef DEBUG_OUTPUT
	qDebug() << "new state" << newState;
#endif
	if(newState != QMediaPlayer::LoadedMedia && newState != QMediaPlayer::BufferedMedia)
		return;

	if(!data_.isEmpty()) {
		ResolvedData data = data_.takeFirst();
		emit newDuration(data.tune, data.duration);
		emit newMetaData(data.tune, data.metaData);
	}

	if(!data_.isEmpty()) {
#ifdef DEBUG_OUTPUT
		qDebug() << "resolve next media";
#endif
		resolveNextMedia();
	}
	else
		resolver_->setMedia(QMediaContent());
}

void QompQtMultimediaMetaDataResolver::totalTimeChanged(qint64 msec)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "duration" << msec;
#endif
	if(!data_.isEmpty()) {
		ResolvedData& data = data_.first();
		data.duration = msec;
	}
}

void QompQtMultimediaMetaDataResolver::error(QMediaPlayer::Error error)
{
	Q_UNUSED(error)
#ifdef DEBUG_OUTPUT
	qDebug() << "error" << error;
#endif
}

void QompQtMultimediaMetaDataResolver::availabilityChanged(bool a)
{
	Q_UNUSED(a)
#ifdef DEBUG_OUTPUT
	qDebug() << "availabilityChanged" << a;
#endif
}

void QompQtMultimediaMetaDataResolver::stateChanged(QMediaPlayer::State state)
{
	Q_UNUSED(state)
#ifdef DEBUG_OUTPUT
	qDebug() << "state changed" << state;
#endif
}

void QompQtMultimediaMetaDataResolver::metaDataAvailableChanged(bool available)
{
	Q_UNUSED(available)
#ifdef DEBUG_OUTPUT
	qDebug() << "metaDataAvailableChanged" << available;
#endif
}

void QompQtMultimediaMetaDataResolver::resolveNextMedia()
{
	resolver_->setMedia(QMediaContent());
	resolver_->setMedia(mediaForTune(data_.first().tune));
	resolver_->pause();
}


QMediaContent QompQtMultimediaMetaDataResolver::mediaForTune(const Tune &tune) const
{
	QMediaContent mc;
	mc = QMediaContent(tune.getUrl());

//		QNetworkRequest nr(url);
//		foreach(const QNetworkCookie& c,
//			QompNetworkingFactory::instance()->getNetworkAccessManager()->cookieJar()->cookiesForUrl(url))
//		{
//			qDebug() << "cookie" << c.toRawForm();
//			nr.setHeader(QNetworkRequest::SetCookieHeader, c.toRawForm());
//		}

	return mc;
}
