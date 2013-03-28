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
		QompPluginTune t = tunes_.at(index.row());
		QString ret = QString("%1 - %2").arg(t.artist, t.title);
		if(!t.url.isNull())
			ret += "  [OK]";
		return ret;
	}

	return QompPluginTracksModel::data(index, role);
}
