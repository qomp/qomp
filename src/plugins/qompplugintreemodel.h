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

class QompPluginModelItem;

class QompPluginTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	QompPluginTreeModel(QObject *parent = 0);
	~QompPluginTreeModel();

	void addTopLevelItems(const QList<QompPluginModelItem*>& items);
	void setItems(const QList<QompPluginModelItem*>& items, QompPluginModelItem* parent);
	void addItems(const QList<QompPluginModelItem*>& items, QompPluginModelItem* parent);

	QompPluginModelItem* itemForId(const QString& id, QompPluginModelItem* parent = 0);
	QList<QompPluginModelItem*> selectedItems() const;
	QModelIndex index(QompPluginModelItem* item) const;
	virtual void reset();

	void emitUpdateSignal();

	//reimplemented from QAbstractItemModel
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

protected:
	void validateSelection(const QModelIndex& parent);

private:
	QSet<QModelIndex> selected_;
	QList<QompPluginModelItem*> topLevelItems_;
};
#endif // QompPluginTreeModel_H
