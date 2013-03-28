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

QompPluginTracksModel::QompPluginTracksModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

void QompPluginTracksModel::addTunes(const QList<QompPluginTune> &tunes)
{
	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size());
	tunes_.append(tunes);
	emit endInsertRows();
}

QompPluginTune QompPluginTracksModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return QompPluginTune();

	return tunes_.at(index.row());
}

QVariant QompPluginTracksModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return QVariant();

	if(index.column() == 0 && role == Qt::CheckStateRole) {
		return QVariant(selected_.contains(index) ? 2 : 0);
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

void QompPluginTracksModel::urlChanged(const QString &id, const QString &url)
{
	QList<QompPluginTune>::iterator it = tunes_.begin();
	for(; it != tunes_.end(); ++it) {
		if((*it).id == id) {
			emit layoutAboutToBeChanged();
			(*it).url = url;
			emit layoutChanged();
			break;
		}
	}
}

void QompPluginTracksModel::reset()
{
	beginResetModel();
	tunes_.clear();
	selected_.clear();
	endResetModel();
}

bool QompPluginTracksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid() || role != Qt::EditRole || index.column() != 0)
		return false;

	switch(value.toInt()) {
	case(DataUnselect):
		if(selected_.contains(index))
			selected_.remove(index);
		break;
	case(DataSelect):
		if(!selected_.contains(index))
			selected_ << index;
		break;
	case(DataToggle):
		if(selected_.contains(index))
			selected_.remove(index);
		else
			selected_ << index;
		break;
	}

	emit dataChanged(index, index);

	return true;
}
