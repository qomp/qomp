/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "qomptaglibmetadataresolver.h"
#include "qompnetworkingfactory.h"
#include "tune.h"
#include "common.h"

#include <taglib/tbytevectorstream.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2framefactory.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const uint tagSize = 5000;


QompTagLibMetaDataResolver::QompTagLibMetaDataResolver(QObject *parent) :
	QompMetaDataResolver(parent),
	nam_(QompNetworkingFactory::instance()->getNetworkAccessManager())
{
}

void QompTagLibMetaDataResolver::start()
{
	resolveNextMedia();
}

void QompTagLibMetaDataResolver::dataReady()
{
	QNetworkReply* r = static_cast<QNetworkReply*>(sender());
	quint64 size = r->size();
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::dataReady()  size=" << QString::number(size);
#endif
	if(size >= tagSize) {
		Tune *tune = get();

		TagLib::ByteVector bv(r->readAll().constData(), size);
		TagLib::ByteVectorStream stream(bv);

		TagLib::FileRef ref(createFile(r->url().toDisplayString(), &stream));
		if(!ref.isNull()) {
			if(ref.tag()) {
				TagLib::Tag* tag = ref.tag();
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::dataReady()  not null tag";
#endif
				tune->artist = Qomp::fixEncoding( tag->artist() );
				tune->album = Qomp::fixEncoding( tag->album() );
				tune->title = Qomp::fixEncoding( tag->title() );
				tune->trackNumber = QString::number( tag->track() );
			}

			if(ref.audioProperties()) {				
				TagLib::AudioProperties *prop = ref.audioProperties();
				if(prop->length())
					tune->duration = Qomp::durationSecondsToString( prop->length() );

				if(prop->bitrate())
					tune->bitRate = QString::number( prop->bitrate() );
			}
		}

		r->abort();
		r->disconnect();
		r->deleteLater();

		tuneFinished();
		resolveNextMedia();
	}
}

void QompTagLibMetaDataResolver::resolveNextMedia()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::resolveNextMedia()";
#endif
	if(!isDataEmpty()) {
		Tune* t = get();
		QNetworkReply* r = nam_->get(QNetworkRequest(t->getUrl()));
		connect(r, SIGNAL(readyRead()), SLOT(dataReady()));
		connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
	}
}

TagLib::File *QompTagLibMetaDataResolver::createFile(const QString& url, TagLib::IOStream* stream)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::createFile   url=" << url;
#endif
	//if(url.endsWith("mp3", Qt::CaseInsensitive)) {
		return new TagLib::MPEG::File(stream, TagLib::ID3v2::FrameFactory::instance());
	//}
	return 0;
}
