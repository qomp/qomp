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

#ifndef COVERCACHE_H
#define COVERCACHE_H

#include <QObject>

class QImage;

class CoverCache : public QObject
{
	Q_OBJECT
public:
	static CoverCache* instance();
	~CoverCache();

	QSharedPointer<QString> put(const QImage& img);
	const QImage get(const QString& hash) const;
	QSharedPointer<QString> restore(const QString& hash);

private:
	CoverCache();
	QByteArray imageToBytes(const QImage &img) const;
	static CoverCache* _instance;

	class Private;
	Private* d;
};

#endif // COVERCACHE_H
