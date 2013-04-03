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

#include "qompplugintracksmodel.h"
#include "qompplugintypes.h"
#include <QIcon>

QompPluginTracksModel::QompPluginTracksModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

QompPluginTracksModel::~QompPluginTracksModel()
{
	reset();
}

void QompPluginTracksModel::addTunes(const QList<QompPluginModelItem *> &tunes)
{
	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size());
	tunes_.append(tunes);
	emit endInsertRows();
}

QompPluginModelItem *QompPluginTracksModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return 0;

	return tunes_.at(index.row());
}

QompPluginModelItem *QompPluginTracksModel::tuneForId(const QString &id) const
{
	foreach(QompPluginModelItem* i, tunes_) {
		if(i->internalId == id)
			return i;
	}
	return 0;
}

QList<QompPluginModelItem *> QompPluginTracksModel::selectedTunes() const
{
	QList<QompPluginModelItem*> list;
	for(int i = 0; i < tunes_.size(); i++) {
		if(selected_.contains(index(i, 0)))
			list.append(tunes_.at(i));
	}
	return list;
}

QVariant QompPluginTracksModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size() || index.column())
		return QVariant();

	if(role == Qt::CheckStateRole) {
		return QVariant(selected_.contains(index) ? 2 : 0);
	}
	else if(role == Qt::DisplayRole) {
		return tunes_.at(index.row())->toString();
	}
	else if(role == Qt::DecorationRole) {
		return tunes_.at(index.row())->icon();
	}
	return QVariant();
}

int QompPluginTracksModel::rowCount(const QModelIndex &/*parent*/) const
{
	return tunes_.size();
}

Qt::ItemFlags QompPluginTracksModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}

void QompPluginTracksModel::reset()
{
	beginResetModel();
	qDeleteAll(tunes_);
	tunes_.clear();
	selected_.clear();
	endResetModel();
}

void QompPluginTracksModel::emitUpdateSignal()
{
	emit layoutAboutToBeChanged();
	emit layoutChanged();
}

bool QompPluginTracksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid() || role != Qt::EditRole || index.column() != 0)
		return false;

	switch(value.toInt()) {
	case(Qomp::DataUnselect):
		if(selected_.contains(index))
			selected_.remove(index);
		break;
	case(Qomp::DataSelect):
		if(!selected_.contains(index))
			selected_ << index;
		break;
	case(Qomp::DataToggle):
		if(selected_.contains(index))
			selected_.remove(index);
		else
			selected_ << index;
		break;
	}

	emit dataChanged(index, index);

	return true;
}
