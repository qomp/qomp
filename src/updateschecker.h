/*
 * updateschecker.h
 * Copyright (C) 2011-2014  Khryukin Evgeny
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

class UpdatesChecker : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool hasUpdate READ hasUpdate  NOTIFY hasUpdateChanged)
public:
	explicit UpdatesChecker(QObject *parent = 0);
	~UpdatesChecker();

	bool hasUpdate() const;

public slots:
	void startCheck(bool interactive = true);

signals:
	void hasUpdateChanged(bool);
	void finished();

private:
	class Private;
	Private* d;
	friend class Private;
	bool hasUpdate_;
};

#endif // UPDATESCHECKER_H
