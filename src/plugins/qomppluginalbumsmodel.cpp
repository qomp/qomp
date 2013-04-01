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

#include "qomppluginalbumsmodel.h"
#include "qompplugintypes.h"


QompPluginAlbumsModel::QompPluginAlbumsModel(QObject *parent) :
	QAbstractItemModel(parent)
{
}

QompPluginAlbumsModel::~QompPluginAlbumsModel()
{
	reset();
}

void QompPluginAlbumsModel::addAlbums(const QList<QompPluginAlbum *> &albums)
{
	emit layoutAboutToBeChanged();
	albums_.append(albums);
	emit layoutChanged();
}

void QompPluginAlbumsModel::setTunes(const QList<QompPluginTune *> &tunes, const QString &parentAlbumId)
{
	foreach(QompPluginAlbum* album, albums_) {
		if(album->id == parentAlbumId) {
			emit layoutAboutToBeChanged();
			qDeleteAll(album->tunes);
			album->tunes.clear();
			album->tunes = tunes;
			validateSelection(indexForAlbum(album));
			emit layoutChanged();
			break;
		}
	}
}

bool QompPluginAlbumsModel::isAlbum(const QModelIndex &index) const
{
	if(index.isValid() && index.internalPointer() == 0)
		return true;

	return false;
}

QompPluginAlbum *QompPluginAlbumsModel::album(const QModelIndex &index) const
{
	if(index.column())
		return 0;

	if(isAlbum(index) && index.row() < albums_.size())
	{
		return albums_.at(index.row());
	}
	return 0;
}

QModelIndex QompPluginAlbumsModel::indexForAlbum(QompPluginAlbum *album) const
{
	if(albums_.contains(album))
		return createIndex(albums_.indexOf(album), 0, (void*)0);

	return QModelIndex();
}

QompPluginAlbum *QompPluginAlbumsModel::album(const QString &id) const
{
	foreach(QompPluginAlbum* a, albums_) {
		if(a->id == id)
			return a;
	}

	return 0;
}

QompPluginTune* QompPluginAlbumsModel::tune(const QModelIndex &index) const
{
	if(index.column() || !index.isValid() || isAlbum(index))
		return 0;

	QModelIndex album = index.parent();
	if(album.isValid() && album.row() < albums_.size() &&
		index.row() < albums_.at(album.row())->tunes.size())
	{
		return albums_.at(album.row())->tunes.at(index.row());
	}
	return 0;
}

QList<QompPluginTune *> QompPluginAlbumsModel::selectedTunes() const
{
	QList<QompPluginTune*> list;
	foreach(const QModelIndex& ind, selected_) {
		if(isAlbum(ind))
			continue;

		list.append(tune(ind));
	}

	return list;
}

bool QompPluginAlbumsModel::urlChanged(const QString &id, const QString &url)
{
	foreach(QompPluginAlbum* album, albums_) {
		foreach(QompPluginTune* tune, album->tunes) {
			if(tune->id == id) {
				emit layoutAboutToBeChanged();
				tune->url = url;
				emit layoutChanged();
				return true;
			}
		}
	}
	return false;
}

QVariant QompPluginAlbumsModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.column())
		return QVariant();


	if(role == Qt::CheckStateRole) {
		return QVariant(selected_.contains(index) ? 2 : 0);
	}

	return QVariant();
}

int QompPluginAlbumsModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid()) {
		if(!isAlbum(parent))
			return 0;

		if(parent.row() < albums_.size()) {
			return albums_.at(parent.row())->tunes.size();
		}
		return 0;
	}

	return albums_.size();
}

int QompPluginAlbumsModel::columnCount(const QModelIndex &/*parent*/) const
{
	return 1;
}

Qt::ItemFlags QompPluginAlbumsModel::flags(const QModelIndex &index) const
{
	if(!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}

bool QompPluginAlbumsModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

	if(isAlbum(index))
		validateSelection(index);
	else
		emit dataChanged(index, index);

	return true;
}

QModelIndex QompPluginAlbumsModel::parent(const QModelIndex &index) const
{
	if(!index.isValid())
		return QModelIndex();

	void* ptr = index.internalPointer();
	if(ptr) {
		for(int a = 0; a < albums_.size(); a++) {
			for(int i = 0; i < albums_.at(a)->tunes.size(); i++) {
				if(ptr == albums_.at(a)->tunes.at(i) ) {
					return createIndex(a, 0, (void*)0);
				}
			}
		}
	}
	return QModelIndex();
}

QModelIndex QompPluginAlbumsModel::index(int row, int column, const QModelIndex &parent) const
{
	if(column)
		return QModelIndex();

	if(!parent.isValid() && row < albums_.size())
		return createIndex(row, column, (void*)0);

	if(parent.isValid() && parent.row() < albums_.size()
		&& row < albums_.at(parent.row())->tunes.size())
	{
		void* ptr = (void*)albums_.at(parent.row())->tunes.at(row);
		return createIndex(row, column, ptr);
	}

	return QModelIndex();
}

void QompPluginAlbumsModel::validateSelection(const QModelIndex &parent)
{
	emit layoutAboutToBeChanged();
	QompPluginAlbum* a = album(parent);
	bool insert = false;
	if(selected_.contains(parent)) {
		insert = true;
	}

	int count = 0;
	foreach(QompPluginTune* t, a->tunes) {
		QModelIndex index = createIndex(count++, 0, t);
		if(insert)
			selected_.insert(index);
		else
			selected_.remove(index);
	}
	emit layoutChanged();
}

void QompPluginAlbumsModel::reset()
{
	beginResetModel();
	qDeleteAll(albums_);
	albums_.clear();
	selected_.clear();
	endResetModel();
}
