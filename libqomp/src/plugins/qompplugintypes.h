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
#include "libqomp_global.h"

namespace QompCon {
enum DataSelection { DataUnselect = 0, DataSelect = 1, DataToggle = 2 };
enum ModelItemType { NoType = 0, TypeTune = 1, TypeAlbum = 2, TypeArtist = 3 };
}
class QIcon;
class Tune;
class QAbstractItemModel;

class LIBQOMPSHARED_EXPORT QompPluginModelItem
{
public:
	/**
	 * Base class for items, that can be stored in QompPluginTreeModel.
	 * Can represent tune, album, artist or something else
	 */
	explicit QompPluginModelItem(QompPluginModelItem* parent = 0);
	virtual ~QompPluginModelItem();

	/**
	* Visual reperesentation of tune item how user will see it in list
	*/
	virtual QString toString() const = 0;

	/**
	 * Tune descrioption used at tooltip
	 */
	virtual QString description() const = 0;

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
	virtual QompCon::ModelItemType type() const = 0;

	/**
	 * Icon for this item (folder icon or file icon etc.)
	 */
#ifdef Q_OS_ANDROID
	virtual QString icon() const = 0;
#else
	virtual QIcon icon() const = 0;
#endif

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

protected:
	static QString makeTooltipString(const QString& key, const QString& value);

private:
	QompPluginModelItem* parent_;
	QList<QompPluginModelItem*> items_;
	QAbstractItemModel* model_;
};

class LIBQOMPSHARED_EXPORT QompPluginTune : public QompPluginModelItem
{
public:
	explicit QompPluginTune(QompPluginModelItem* parent = 0);

	QString title;
	QString artist;
	QString album;
	QString duration;
	QString url;

	virtual QString toString() const Q_DECL_OVERRIDE;
	virtual QString description() const Q_DECL_OVERRIDE;
	virtual QompCon::ModelItemType type() const Q_DECL_OVERRIDE;
#ifdef Q_OS_ANDROID
	QString
#else
	QIcon
#endif
	virtual icon() const Q_DECL_OVERRIDE;
	virtual Tune* toTune() const;
};

class LIBQOMPSHARED_EXPORT QompPluginAlbum : public QompPluginModelItem
{
public:
	explicit QompPluginAlbum(QompPluginModelItem* parent = 0);

	QString album;
	QString artist;
	QString year;
	bool tunesReceived;

	virtual QString toString() const Q_DECL_OVERRIDE;
	virtual QString description() const Q_DECL_OVERRIDE;
	virtual QompCon::ModelItemType type() const Q_DECL_OVERRIDE;
#ifdef Q_OS_ANDROID
	virtual QString icon() const Q_DECL_OVERRIDE;
#else
	virtual QIcon icon() const Q_DECL_OVERRIDE;
#endif
};

class LIBQOMPSHARED_EXPORT QompPluginArtist : public QompPluginModelItem
{
public:
	explicit QompPluginArtist(QompPluginModelItem* parent = 0);

	QString artist;
	bool tunesReceived;

	virtual QString toString() const Q_DECL_OVERRIDE;
	virtual QString description() const Q_DECL_OVERRIDE;
	virtual QompCon::ModelItemType type() const Q_DECL_OVERRIDE;
#ifdef Q_OS_ANDROID
	virtual QString icon() const Q_DECL_OVERRIDE;
#else
	virtual QIcon icon() const Q_DECL_OVERRIDE;
#endif
};

#endif // QOMPPLUGINTYPES_H
