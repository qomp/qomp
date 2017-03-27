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

class CoverCache::Private
{
public:
	Private();
	~Private();

	void loadCache();
	void saveCache();
	void cleanup();

	QList< QPair<QWeakPointer<QString>, QImage> > data;
	QFile* cache;
};



CoverCache::Private::Private()
{
	cache = new QFile(Qomp::cacheDir() + cacheName);
	if(!cache->open(QFile::ReadWrite)) {
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
		QHash<QString,QImage> hash;
		ds >> hash;

		for(auto& pair: data) {
			if(!pair.first.isNull() && hash.contains(*pair.first.data())) {
				pair.second = hash.value(*pair.first.data());
			}
		}
	}
}

void CoverCache::Private::saveCache()
{
	if(cache) {
		cache->resize(0);
		QDataStream ds(cache);
		QHash<QString,QImage> hash;
		for(const auto& pair: data) {
			if(!pair.first.isNull())
				hash.insert(*pair.first.data(), pair.second);
		}
		ds << hash;
	}
}

void CoverCache::Private::cleanup()
{
	for(int i = data.count()-1; i>=0; i--) {
		if (data.at(i).first.isNull())
			data.removeAt(i);
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
	QByteArray res = hash.result();

	for(const auto& pair: d->data) {
		if(!pair.first.isNull() && pair.first.data()->compare(res) == 0) {
			return pair.first.toStrongRef();
		}
	}

	QSharedPointer<QString> p(new QString(res));
	d->data.append({ p.toWeakRef(), img });
	return p;
}

const QImage CoverCache::get(const QString &hash) const
{
	for(const auto& pair: d->data) {
		if(!pair.first.isNull() && pair.first.data()->compare(hash) == 0) {
			return pair.second;
		}
	}

	return QImage();
}

QSharedPointer<QString> CoverCache::restore(const QString &hash)
{
	if(d->cache) {
		d->cache->seek(0);
		QDataStream ds(d->cache);
		QHash<QString,QImage> h;
		ds >> h;

		for(const QString& key: h.keys()) {
			if(key.compare(hash) == 0) {
				return put(h.value(key));
			}
		}
	}

	return QSharedPointer<QString>();
}

CoverCache* CoverCache::_instance = nullptr;
