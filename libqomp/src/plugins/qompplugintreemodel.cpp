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

#include "qompplugintreemodel.h"
#include "qompplugintypes.h"
#include <QIcon>


QompPluginTreeModel::QompPluginTreeModel(QObject *parent) :
	QAbstractItemModel(parent)
{
}

QompPluginTreeModel::~QompPluginTreeModel()
{
	reset();
}

void QompPluginTreeModel::addTopLevelItems(const QList<QompPluginModelItem *> &items)
{
	if(items.size() > 0) {
		beginInsertRows(QModelIndex(), topLevelItems_.size(), topLevelItems_.size() + items.size()-1);
		topLevelItems_.append(items);
		foreach(QompPluginModelItem* item,  items) {
			item->setModel(this);
		}
		endInsertRows();
	}
}

void QompPluginTreeModel::setItems(const QList<QompPluginModelItem *> &items, QompPluginModelItem *parent)
{
	QModelIndex ind = index(parent);
	if(!ind.isValid())
		return;

	if(parent->items().size() > 0) {
		beginRemoveRows(ind, 0, parent->items().size()-1);
		foreach(QompPluginModelItem* it, parent->items()) {
			selected_.remove(index(it));
		}
		parent->setItems(QList<QompPluginModelItem *>());
		endRemoveRows();
	}

	if(items.size() > 0) {
		beginInsertRows(ind, 0, items.size()-1);
		parent->setItems(items);
		validateSelection(ind);
		//	foreach(QompPluginModelItem* item,  items) {
		//		item->setModel(this);
		//	}
		endInsertRows();
	}
}

void QompPluginTreeModel::addItems(const QList<QompPluginModelItem *> &items, QompPluginModelItem *parent)
{
	QModelIndex ind = index(parent);
	if(ind.isValid() && items.size() > 0) {
		beginInsertRows(ind, parent->items().size(), parent->items().size() + items.size()-1);
		parent->addItems(items);
		//	foreach(QompPluginModelItem* item,  items) {
		//		item->setModel(this);
		//	}
		endInsertRows();
	}
}

QompPluginModelItem *QompPluginTreeModel::itemForId(const QString &id, QompPluginModelItem *parent)
{
	if(parent && parent->internalId == id)
		return parent;

	QList<QompPluginModelItem *> items = parent ? parent->items() : topLevelItems_;
	foreach(QompPluginModelItem* it, items) {
		if(it->internalId == id)
			return it;

		QompPluginModelItem* mi = itemForId(id, it);
		if(mi)
			return mi;
	}

	return 0;
}

QompPluginModelItem *QompPluginTreeModel::item(const QModelIndex &index) const
{
	if(!index.isValid())
		return 0;

	return (QompPluginModelItem *)index.internalPointer();
}

QList<QompPluginModelItem*> QompPluginTreeModel::selectedItems(QompPluginModelItem *parent) const
{
	QList<QompPluginModelItem*> list;
	QList<QompPluginModelItem*> items = parent ? parent->items() : topLevelItems_;
	for(int i = 0; i < items.size(); i++) {
		if(selected_.contains(index(items.at(i))))
			list.append(items.at(i));

		list.append(selectedItems(items.at(i)));
	}

	return list;
}

QVariant QompPluginTreeModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.column())
		return QVariant();


	if(role == Qt::CheckStateRole) {
		return QVariant(selected_.contains(index) ? 2 : 0);
	}
	else if(role == Qt::DisplayRole) {
		return ((QompPluginModelItem*)index.internalPointer())->toString();
	}
	else if(role == Qt::DecorationRole) {
		return ((QompPluginModelItem*)index.internalPointer())->icon();
	}
	return QVariant();
}

int QompPluginTreeModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid()) {
		return ((QompPluginModelItem*)parent.internalPointer())->items().size();
	}

	return topLevelItems_.size();
}

int QompPluginTreeModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}

Qt::ItemFlags QompPluginTreeModel::flags(const QModelIndex &index) const
{
	if(!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}

bool QompPluginTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid() || role != Qt::CheckStateRole || index.column() != 0)
		return false;

	layoutAboutToBeChanged();

	switch(value.toInt()) {
	case(QompCon::DataUnselect):
		if(selected_.contains(index))
			selected_.remove(index);
		break;
	case(QompCon::DataSelect):
		if(!selected_.contains(index))
			selected_ << index;
		break;
	case(QompCon::DataToggle):
		if(selected_.contains(index))
			selected_.remove(index);
		else
			selected_ << index;
		break;
	}

	validateSelection(index);

	layoutChanged();
	return true;
}

QModelIndex QompPluginTreeModel::parent(const QModelIndex &index) const
{
	if(!index.isValid())
		return QModelIndex();

	if(!index.internalPointer())
		return QModelIndex();

	QompPluginModelItem* it = (QompPluginModelItem*)index.internalPointer();
	if(topLevelItems_.contains(it))
		return QModelIndex();

	QompPluginModelItem* par = (QompPluginModelItem*)it->parent();
	if(par) {
		if(topLevelItems_.contains(par))
			return createIndex(topLevelItems_.indexOf(par), 0, par);

		QompPluginModelItem* par2 = par->parent();
		if(par2) {
			return createIndex(par2->items().indexOf(par), 0, par);
		}
	}

	return QModelIndex();
}

QModelIndex QompPluginTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if(column)
		return QModelIndex();

	if(parent.isValid() && parent.internalPointer()) {
		QompPluginModelItem* par = (QompPluginModelItem*)parent.internalPointer();
		if(row < par->items().size())
			return createIndex(row, column, par->items().at(row));
	}
	else if(row < topLevelItems_.size()) {
		return createIndex(row, column, topLevelItems_.at(row));
	}

	return QModelIndex();
}

QHash<int, QByteArray> QompPluginTreeModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[Qt::DisplayRole] = "text";
	roles[Qt::CheckStateRole] = "state";
	roles[Qt::DecorationRole] = "icon";
	return roles;
}

QModelIndex QompPluginTreeModel::index(QompPluginModelItem *item) const
{
	if(!item)
		return QModelIndex();

	if(item->parent()) {
		return createIndex(item->parent()->items().indexOf(item), 0, item);
	}
	if(topLevelItems_.contains(item))
		return createIndex(topLevelItems_.indexOf(item), 0, item);

	return QModelIndex();
}

void QompPluginTreeModel::validateSelection(const QModelIndex &parent)
{
	QompPluginModelItem* item = (QompPluginModelItem*)parent.internalPointer();
	bool select = selected_.contains(parent);
	foreach(QompPluginModelItem* it, item->items()) {
		QModelIndex i = index(it);
		if(select)
			selected_.insert(i);
		else
			selected_.remove(i);

		validateSelection(i);
	}
}

void QompPluginTreeModel::reset()
{
	beginResetModel();
	qDeleteAll(topLevelItems_);
	topLevelItems_.clear();
	selected_.clear();
	endResetModel();
}

void QompPluginTreeModel::emitUpdateSignal()
{
	emit layoutAboutToBeChanged();
	emit layoutChanged();
}
