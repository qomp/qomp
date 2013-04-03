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
#include <QString>

namespace Qomp {
enum DataSelection { DataUnselect = 0, DataSelect = 1, DataToggle = 2 };
enum ModelItemType { NoType = 0, TypeTune, TypeAlbum, TypeArtist };
}
class QIcon;

class QompPluginModelItem
{
public:
	QompPluginModelItem(QompPluginModelItem* parent = 0);
	virtual ~QompPluginModelItem();

	virtual QString toString() const = 0;
	void setParent(QompPluginModelItem* parent);
	QompPluginModelItem* parent() const;
	void setItems(QList<QompPluginModelItem*> items);
	void addItems(QList<QompPluginModelItem*> items);
	QList<QompPluginModelItem*> items() const;
	virtual Qomp::ModelItemType type() const = 0;
	virtual QIcon icon() const = 0;

	QString internalId;

private:
	QompPluginModelItem* parent_;
	QList<QompPluginModelItem*> items_;
};

class QompPluginTune : public QompPluginModelItem
{
public:
	QompPluginTune(QompPluginModelItem* parent = 0);

	QString title;
	QString artist;
	QString album;
	QString duration;
	QString url;

	virtual QString toString() const;
	virtual Qomp::ModelItemType type() const;
	QIcon icon() const;
};

class QompPluginAlbum : public QompPluginModelItem
{
public:
	QompPluginAlbum(QompPluginModelItem* parent = 0);

	QString album;
	QString artist;
	QString year;
	bool tunesReceived;

	virtual QString toString() const;
	virtual Qomp::ModelItemType type() const;
	QIcon icon() const;
};

class QompPluginArtist : public QompPluginModelItem
{
public:
	QompPluginArtist(QompPluginModelItem* parent = 0);

	QString artist;
	bool tunesReceived;

	virtual QString toString() const;
	virtual Qomp::ModelItemType type() const;
	QIcon icon() const;
};

#endif // QOMPPLUGINTYPES_H
