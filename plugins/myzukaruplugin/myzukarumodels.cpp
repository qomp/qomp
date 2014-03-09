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


#include "myzukarumodels.h"
#include "qompplugintypes.h"
#include "common.h"


MyzukaruArtistsModel::MyzukaruArtistsModel(QObject *parent)
	: QompPluginTreeModel(parent)
{
}

Qt::ItemFlags MyzukaruArtistsModel::flags(const QModelIndex &index) const
{
	if(!index.isValid())
		return Qt::NoItemFlags;

	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	if(index.parent().isValid())
		flags |= Qt::ItemIsUserCheckable;

	return  flags;
}

QVariant MyzukaruArtistsModel::data(const QModelIndex &index, int role) const
{
	if(role == Qt::CheckStateRole &&
		index.isValid() && !index.parent().isValid()) {
		return QVariant();
	}
	return QompPluginTreeModel::data(index, role);
}

bool MyzukaruArtistsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(role == Qt::EditRole &&
		index.isValid() && !index.parent().isValid())
		return false;

	return QompPluginTreeModel::setData(index, value, role);
}

