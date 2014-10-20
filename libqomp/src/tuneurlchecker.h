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

#ifndef TUNEURLCHECKER_H
#define TUNEURLCHECKER_H

#include <QObject>
#include <QUrl>

#include "libqomp_global.h"

class QNetworkAccessManager;
class QEventLoop;
class QTimer;
class QNetworkReply;

class LIBQOMPSHARED_EXPORT TuneUrlChecker : public QObject
{
	Q_OBJECT
public:
	TuneUrlChecker(QNetworkAccessManager* nam, const QUrl& url, QObject *parent = 0);
	bool result() const;

private slots:
	void readyRead();
	void error();
	void timeout();

private:
	QNetworkAccessManager* nam_;
	bool result_;
	QEventLoop* loop_;
	QUrl url_;
	QTimer* timer_;
	mutable QNetworkReply* reply_;
};

#endif // TUNEURLCHECKER_H
