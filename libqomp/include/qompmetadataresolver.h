/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#ifndef QOMPMETADATARESOLVER_H
#define QOMPMETADATARESOLVER_H

#include <QObject>
//#include <QMap>
#include "libqomp_global.h"

class Tune;

class LIBQOMPSHARED_EXPORT QompMetaDataResolver : public QObject
{
	Q_OBJECT
public:
	QompMetaDataResolver(QObject *parent = 0);
	~QompMetaDataResolver();

	void resolve(const QList<Tune*>& tunes);
	
signals:
	void tuneUpdated(Tune*);
	
protected:
	Tune *get();
	void tuneFinished();
	bool isDataEmpty() const;

protected slots:
	virtual void start() = 0;

//protected:
//	void updateTuneMetadata(const QMap<QString, QString>& data);
//	void updateTuneDuration(qint64 msec);

private:
	void addTunes(const QList<Tune*>& tunes);

private:
	QList<Tune*> data_;
};

#endif // QOMPMETADATARESOLVER_H
