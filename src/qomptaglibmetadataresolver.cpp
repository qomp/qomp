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
//#include <QNetworkCookieJar>
//#include <QAbstractNetworkCache>
//#include <QNetworkConfiguration>
#include <QNetworkProxy>
#include <QApplication>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const uint tagSize = 5000;


QompTagLibMetaDataResolver::QompTagLibMetaDataResolver(QObject *parent) :
	QompMetaDataResolver(parent),
	nam_(0)
{
}

void QompTagLibMetaDataResolver::run()
{
	nam_ = new QNetworkAccessManager();
	QNetworkAccessManager* m = QompNetworkingFactory::instance()->getNetworkAccessManager();

//	nam_->setCache(m->cache());
//	nam_->cache()->setParent(m);

//	nam_->setConfiguration(m->configuration());

//	nam_->setCookieJar(m->cookieJar());
//	nam_->cookieJar()->setParent(m);

	nam_->setProxy(m->proxy());
	connect(nam_, SIGNAL(finished(QNetworkReply*)), SLOT(requestFinished(QNetworkReply*)));

	resolveNextMedia();
	exec();
	nam_->deleteLater();
	nam_ = 0;
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
	qDebug() << "QompTagLibMetaDataResolver::dataReady()  not null tag   " << QString::fromStdWString( tag->title().toWString() );
#endif
				tune->artist = Qomp::fixEncoding(QString::fromStdWString( tag->artist().toWString() ));
				tune->album = Qomp::fixEncoding(QString::fromStdWString( tag->album().toWString() ));
				tune->title = Qomp::fixEncoding(QString::fromStdWString( tag->title().toWString() ));
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

void QompTagLibMetaDataResolver::requestFinished(QNetworkReply *r)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::requestFinished";
#endif
	r->deleteLater();
}

void QompTagLibMetaDataResolver::resolveNextMedia()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompTagLibMetaDataResolver::resolveNextMedia()";
#endif
	if(isDataEmpty()) {
		exit();
	}
	else {
		Tune* t = get();
		QNetworkReply* r = nam_->get(QNetworkRequest(t->getUrl()));
		connect(r, SIGNAL(readyRead()), SLOT(dataReady()));
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
