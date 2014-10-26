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

#ifndef QOMPTUNEDOWNLOADER_H
#define QOMPTUNEDOWNLOADER_H

#include <QObject>

class Tune;

class QompTuneDownloader : public QObject
{
	Q_OBJECT
public:
	explicit QompTuneDownloader(QObject *parent = 0);
	~QompTuneDownloader();

	void download(Tune* tune, const QString& dir);
	
private slots:
	void finished();
	void readyRead();
	void abort();

private:
	class Private;
	Private* d;
	friend class Private;
};

#endif // QOMPTUNEDOWNLOADER_H
