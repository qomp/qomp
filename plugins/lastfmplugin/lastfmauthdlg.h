/*
 * Copyright (C) 2013  Khryukin Evgeny
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


#ifndef LASTFMAUTHDLG_H
#define LASTFMAUTHDLG_H

#include <QObject>

class LastFmAuthDlg : public QObject
{
	Q_OBJECT
	
public:
	explicit LastFmAuthDlg(QObject *parent = 0);
	~LastFmAuthDlg();

	enum Result {Accepted = 1, Rejected = 0 };
	Result openUrl(const QString& url);

private slots:
	void openUrl();
	
private:
	class Private;
	friend class Private;
	Private *d;
	QString url_;
};

#endif // LASTFMAUTHDLG_H
