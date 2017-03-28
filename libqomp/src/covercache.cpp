/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#include "covercache.h"
#include "common.h"

#include <QCoreApplication>
#include <QImage>
#include <QHash>
#include <QSharedPointer>
#include <QFile>
#include <QCryptographicHash>


static const QString cacheName = "/image_cache";

typedef QPair<QWeakPointer<QString>, QImage> CoverPair;

class CoverCache::Private
{
public:
	Private();
	~Private();

	void loadCache();
	void saveCache();
	void cleanup();

	QHash<QString, CoverPair> data;
	QFile* cache;
};



CoverCache::Private::Private()
{
	cache = new QFile(Qomp::cacheDir() + cacheName);
	if(cache->open(QFile::ReadWrite)) {
		loadCache();
	}
	else {
		delete cache;
		cache = nullptr;
	}
}

CoverCache::Private::~Private()
{
	saveCache();
	delete cache;
}

void CoverCache::Private::loadCache()
{
	if(cache) {
		QDataStream ds(cache);
		while(!ds.atEnd()) {
			QString hash;
			QImage img;
			ds >> hash >> img;
			CoverPair pair{ QWeakPointer<QString>(), img };
			data.insert(hash, pair);
		}
	}
}

void CoverCache::Private::saveCache()
{
	if(cache) {
		cache->resize(0);
		QDataStream ds(cache);
		for(const QString& hash: data.keys()) {
			const CoverPair pair = data.value(hash);
			if(!pair.first.isNull())
				ds << hash << pair.second;
		}
	}
}

void CoverCache::Private::cleanup()
{
	for(const QString& hash: data.keys()) {
		const CoverPair pair = data.value(hash);
		if(pair.first.isNull())
			data.remove(hash);
	}
}




CoverCache::CoverCache() :
	QObject(qApp),
	d(new Private)
{
}

QByteArray CoverCache::imagetToBytes(const QImage &img) const
{
	QByteArray ba;
	QDataStream ds(ba);
	ds << img;
	return ba;
}

CoverCache *CoverCache::instance()
{
	if(!_instance)
		_instance = new CoverCache;

	return _instance;
}

CoverCache::~CoverCache()
{
	delete d;
}

QSharedPointer<QString> CoverCache::put(const QImage &img)
{
	d->cleanup();

	QCryptographicHash hash(QCryptographicHash::Sha1);
	hash.addData(imagetToBytes(img));
	const QString res(hash.result());

	if(d->data.contains(res)) {
		const CoverPair pair = d->data.value(res);
		if(!pair.first.isNull()) {
			return pair.first.toStrongRef();
		}		
	}

	QSharedPointer<QString> p(new QString(res));
	const CoverPair pair{ p.toWeakRef(), img };
	d->data.insert(res, pair);

	return p;
}

const QImage CoverCache::get(const QString &hash) const
{
	if(d->data.contains(hash)) {
		const CoverPair pair = d->data.value(hash);
		if(!pair.first.isNull()) {
			return pair.second;
		}
	}

	return QImage();
}

QSharedPointer<QString> CoverCache::restore(const QString &hash)
{
	if(d->data.contains(hash)) {
		CoverPair pair = d->data.value(hash);
		QSharedPointer<QString> p(new QString(hash));
		pair.first = p.toWeakRef();
		d->data.insert(hash, pair);
		return p;
	}

	return QSharedPointer<QString>();
}

CoverCache* CoverCache::_instance = nullptr;
