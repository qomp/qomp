/*
 * Copyright (C) 2014-2017  Khryukin Evgeny
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
#include "taghelpers.h"
#include "common.h"

#ifndef Q_OS_MAC
#include <taglib/tbytevectorstream.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2framefactory.h>
#include <taglib/id3v2tag.h>
#else
#include <tag/tbytevectorstream.h>
#include <tag/fileref.h>
#include <tag/tag.h>
#include <tag/audioproperties.h>
#include <tag/mpegfile.h>
#include <tag/id3v2framefactory.h>
#include <tag/id3v2tag.h>
#endif

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const uint tagSize = 100;


QompTagLibMetaDataResolver::QompTagLibMetaDataResolver(QObject *parent) :
	QompMetaDataResolver(parent),
	nam_(QompNetworkingFactory::instance()->getMainNAM()),
	fullSize_(tagSize),
	step_(0)
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
	if (step_ == 0) {
		if(size > tagSize) {
			loadFullSize(r->url(), r->peek(size));
			step_ = 1;
		}
	}
	else if(size >= fullSize_) {
		TagLib::ByteVector bv(r->readAll().constData(), size);

		r->disconnect();
		r->abort();
		r->deleteLater();

		processData(r->url(), bv);
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
		if(t->isMetadataResolved()) {
			tuneFinished();
			resolveNextMedia();
			return;
		}

		QUrl u(t->getUrl());
		if(!u.isLocalFile()) {
			QNetworkReply* r = nam_->get(QNetworkRequest(u));

			connect(r, &QNetworkReply::readyRead, this, &QompTagLibMetaDataResolver::dataReady);

			//for some cases if some went wrong
			connect(r, &QNetworkReply::finished, [r, this]() {
				r->disconnect();
				r->deleteLater();
				tuneFinished();
				resolveNextMedia();
			});
		}
		else {
			auto ref = Qomp::fileName2TaglibRef(u.toLocalFile(), false);
			Qomp::loadCover(t, ref.file());
			tuneFinished();
			resolveNextMedia();
		}
		step_ = 0;
	}
}

TagLib::MPEG::File *QompTagLibMetaDataResolver::createFile(const QString& url, TagLib::IOStream* stream)
{
	Q_UNUSED(url)
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::createFile   url=" << url;
#endif
	//if(url.endsWith("mp3", Qt::CaseInsensitive)) {
		stream->seek(0);
		return new TagLib::MPEG::File(stream, TagLib::ID3v2::FrameFactory::instance());
	//}
		return 0;
}

void QompTagLibMetaDataResolver::processData(const QUrl &url, const TagLib::ByteVector &data)
{
	TagLib::ByteVectorStream stream(data);
	auto tagFile = createFile(url.toDisplayString(), &stream);

	TagLib::FileRef ref(tagFile);
	if(tagFile->isValid() && !ref.isNull()) {
		Tune *tune = get();
		Qomp::loadCover(tune, tagFile);

		if(ref.audioProperties()) {
			TagLib::AudioProperties *prop = ref.audioProperties();
			if(prop->length())
				tune->duration = Qomp::durationSecondsToString( prop->length() );

			if(prop->bitrate())
				tune->bitRate = QString::number( prop->bitrate() );
		}
	}
}

void QompTagLibMetaDataResolver::loadFullSize(const QUrl& url, const QByteArray &data)
{
	TagLib::ByteVector bv(data.constData(), data.size());
	TagLib::ByteVectorStream stream(bv);

	auto tagFile = createFile(url.toDisplayString(), &stream);
	TagLib::FileRef ref(tagFile);
	if(!ref.isNull()) {
		auto tag = tagFile->ID3v2Tag(false);
		fullSize_ = tag->header()->completeTagSize();
	}
}
