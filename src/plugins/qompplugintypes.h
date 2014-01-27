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
class Tune;
class QAbstractItemModel;

class QompPluginModelItem
{
public:
	/**
	 * Base class for items, that can be stored in QompPluginTreeModel.
	 * Can represent tune, album, artist or something else
	 */
	QompPluginModelItem(QompPluginModelItem* parent = 0);
	virtual ~QompPluginModelItem();

	/**
	* Visual reperesentation of tune item how user will see it in list
	*/
	virtual QString toString() const = 0;

	/**
	* Set parent for this item. When parent item will be deleted
	* all children will be deleted too
	*/
	void setParent(QompPluginModelItem* parent);

	/**
	 * Returns pointer on parent item
	 */
	QompPluginModelItem* parent() const;

	/**
	* list of child items
	*/
	QList<QompPluginModelItem*> items() const;

	/**
	 * Type of this item (tune, album, artist)
	 */
	virtual Qomp::ModelItemType type() const = 0;

	/**
	 * Icon for this item (folder icon or file icon etc.)
	 */
	virtual QIcon icon() const = 0;

	/**
	* Internal pointer to item, depending on music hosting.
	* Can be an url, or some internal id etc.
	*/
	QString internalId;

	/**
	 * Set list of child items. This item takes ownership and will delete
	 * them in destructor. Old children will be deleted
	 */
	void setItems(QList<QompPluginModelItem*> items);

	/**
	 * Append list of child items. This item takes ownership and will delete
	 * them in destructor.
	 */
	void addItems(QList<QompPluginModelItem*> items);

	QAbstractItemModel* model() const;
	void setModel(QAbstractItemModel* model);

private:
	QompPluginModelItem* parent_;
	QList<QompPluginModelItem*> items_;
	QAbstractItemModel* model_;
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
	virtual Tune* toTune() const;
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
