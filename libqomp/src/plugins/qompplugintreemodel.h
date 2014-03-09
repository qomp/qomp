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

#ifndef QompPluginTreeModel_H
#define QompPluginTreeModel_H

#include <QAbstractItemModel>
#include <QSet>
#include "libqomp_global.h"

class QompPluginModelItem;

class LIBQOMPSHARED_EXPORT QompPluginTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	/**
	 * Class designed for hold and manage search results.
	 * Represents a tree of QompPluginModelItem's
	 */
	QompPluginTreeModel(QObject *parent = 0);

	/**
	 * Destructor removes all top level items.
	 * Each top level item removes it's children
	 */
	~QompPluginTreeModel();

	/**
	 * Add list of <QompPluginModelItem's like top level items
	 */
	void addTopLevelItems(const QList<QompPluginModelItem*>& items);

	/**
	 * Set list of QompPluginModelItem's for parent QompPluginModelItem.
	 * Old items will be deleted
	 */
	void setItems(const QList<QompPluginModelItem*>& items, QompPluginModelItem* parent);

	/**
	 * Append list of QompPluginModelItem's for parent QompPluginModelItem.
	 */
	void addItems(const QList<QompPluginModelItem*>& items, QompPluginModelItem* parent);

	/**
	 * Return pointer on QompPluginModelItem, which has given internalId and parent
	 * If parent not given recursive search over all items will be performed
	 * Return 0 if not found.
	 */
	QompPluginModelItem* itemForId(const QString& id, QompPluginModelItem* parent = 0);

	/**
	 * Return pointer on QompPluginModelItem, which represented by given QModelIndex.
	 * Return 0 if index not valid
	 */
	QompPluginModelItem* item(const QModelIndex& index);

	/**
	 * Return list of selected items
	 */
	QList<QompPluginModelItem*> selectedItems(QompPluginModelItem* parent = 0) const;

	/**
	 * Return QModelIndex, which representing given  QompPluginModelItem
	 */
	QModelIndex index(QompPluginModelItem* item) const;

	/**
	 * Delete all stored items
	 */
	virtual void reset();

	/**
	 * Notify connected views than some items inside has changed
	 */
	void emitUpdateSignal();

	/**
	* Reimplemented from QAbstractItemModel
	* Take a look into Qt's documentation
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

protected:
	/**
	* If we select or unselect parent item,
	* all his children should be selected or unselected too
	*/
	void validateSelection(const QModelIndex& parent);

protected:
	QSet<QModelIndex> selected_;
	QList<QompPluginModelItem*> topLevelItems_;
};
#endif // QompPluginTreeModel_H
