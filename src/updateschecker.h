/*
 * updateschecker.h
 * Copyright (C) 2011  Khryukin Evgeny
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

#ifndef UPDATESCHECKER_H
#define UPDATESCHECKER_H

#include <QObject>

class QNetworkReply;
class QProgressDialog;

class UpdatesChecker : public QObject
{
	Q_OBJECT
public:
	explicit UpdatesChecker(QObject *parent = 0);
	~UpdatesChecker();

private:
	void showError(const QString& error);

private slots:
	void replyFinished(QNetworkReply*);
	void updateProgress(qint64,qint64);

private:
	QProgressDialog* progressDialog_;
};

#endif // UPDATESCHECKER_H
