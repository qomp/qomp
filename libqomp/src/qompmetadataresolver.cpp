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

#include "qompmetadataresolver.h"
#include "tune.h"
#include "common.h"

#include <QTimer>

QompMetaDataResolver::QompMetaDataResolver(QObject *parent) :
	QObject(parent)
{
}

QompMetaDataResolver::~QompMetaDataResolver()
{
}

void QompMetaDataResolver::resolve(const QList<Tune*> &tunes)
{
	bool s = data_.isEmpty();
	addTunes(tunes);
	if(s) {
		QTimer::singleShot(100, this, SLOT(start()));
	}
}

Tune* QompMetaDataResolver::get()
{
	return data_.first();
}

void QompMetaDataResolver::tuneFinished()
{
	Tune* t = const_cast<Tune*>(Tune::emptyTune());

	if(!data_.isEmpty()) {
		t = data_.takeFirst();
	}

	if(t != Tune::emptyTune())
		emit tuneUpdated(t);
}

bool QompMetaDataResolver::isDataEmpty() const
{
	return data_.isEmpty();
}

//void QompMetaDataResolver::updateTuneMetadata(const QMap<QString, QString> &data)
//{
//	if(data.isEmpty())
//		return;

//	QString tmp = Qomp::fixEncoding(data.value("ARTIST"));
//	Tune* t = data_.first();
//	if(!tmp.isEmpty())
//		t->artist = tmp;

//	tmp = Qomp::fixEncoding(data.value("ALBUM"));
//	if(!tmp.isEmpty())
//		t->album = tmp;

//	tmp = Qomp::fixEncoding(data.value("TITLE"));
//	if(!tmp.isEmpty())
//		t->title = tmp;

//	tmp = data.value("TRACK-NUMBER");
//	if(!tmp.isEmpty())
//		t->trackNumber = tmp;

//	tmp = data.value("BITRATE");
//	if(!tmp.isEmpty())
//		t->bitRate = tmp;
//}

//void QompMetaDataResolver::updateTuneDuration(qint64 msec)
//{
//	if(msec == -1 || msec == 0)
//		return;

//	data_.first()->duration = Qomp::durationMiliSecondsToString(msec);
//}

void QompMetaDataResolver::addTunes(const QList<Tune*> &tunes)
{
	data_.append(tunes);
}
