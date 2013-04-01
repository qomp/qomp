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


MyzukaruTracksModel::MyzukaruTracksModel(QObject *parent) :
	QompPluginTracksModel(parent)
{
}

QVariant MyzukaruTracksModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return QVariant();

	if(index.column() == 0 && role == Qt::DisplayRole) {
		QompPluginTune* t = tunes_.at(index.row());
		QString ret = QString("%1 - %2").arg(t->artist, t->title);
		if(!t->url.isNull())
			ret += "  [OK]";
		return ret;
	}

	return QompPluginTracksModel::data(index, role);
}





MyzukaruAlbumsModel::MyzukaruAlbumsModel(QObject *parent) :
	QompPluginAlbumsModel(parent)
{
}


QVariant MyzukaruAlbumsModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.column())
		return QVariant();

	if(role == Qt::DisplayRole)
	{
		if(isAlbum(index) && index.row() < albums_.size()) {
			QompPluginAlbum* a = albums_.at(index.row());
			return	a->artist + " - " +
				a->album + " - " +
				a->year + " [" +
				QString::number(rowCount(index))/*QString::number(a.tunes.size())*/ + "]";
		}
		QompPluginTune* t = tune(index);
		if(t) {
			QString ret = QString("%1 - %2").arg(t->artist, t->title);
			if(!t->url.isNull())
				ret += "  [OK]";
			return ret;
		}
	}

	return QompPluginAlbumsModel::data(index, role);
}


MyzukaruArtistsModel::MyzukaruArtistsModel(QObject *parent)
	: QompPluginAlbumsModel(parent)
{
}

QVariant MyzukaruArtistsModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.column())
		return QVariant();

	if(role == Qt::DisplayRole)
	{
		if(isAlbum(index) && index.row() < albums_.size()) {
			QompPluginAlbum* a = albums_.at(index.row());
			return	a->artist + " - [" +
				QString::number(rowCount(index)) + "]";
		}
		QompPluginTune* t = tune(index);
		if(t) {
			QString ret = QString("%1 - %2").arg(t->artist, t->title);
			if(!t->url.isNull())
				ret += "  [OK]";
			return ret;
		}
	}

	return QompPluginAlbumsModel::data(index, role);
}
