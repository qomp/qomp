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

#ifndef QOMPPLUGINTYPES_H
#define QOMPPLUGINTYPES_H

#include <QList>

enum DataSelection { DataUnselect = 0, DataSelect = 1, DataToggle = 2 };


struct QompPluginTune
{
	QString title;
	QString artist;
	QString album;
	QString duration;
	QString id;
	QString url;

//	bool operator==(const QompPluginTune& other);
};

struct QompPluginAlbum
{
	QompPluginAlbum() : tunesReceived(false) {}
	~QompPluginAlbum()
	{
		qDeleteAll(tunes);
	}

	QString album;
	QString artist;
	QString year;
	QString id;
	bool tunesReceived;
	QList<QompPluginTune*> tunes;

//	bool operator==(const QompPluginAlbum& other);
};

#endif // QOMPPLUGINTYPES_H
