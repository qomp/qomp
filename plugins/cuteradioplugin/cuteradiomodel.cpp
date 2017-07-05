/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#include "cuteradiomodel.h"

CuteRadioModel::CuteRadioModel(QObject *parent) :
	QompPluginTreeModel(parent)
{

}

QVariant CuteRadioModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.column())
		return QVariant();

	if(role == Qt::ToolTipRole) {
		auto t = static_cast<CuteRadioTune*>(index.internalPointer());
		QString ret = QString("<div>%1</div>"
				      "<div><nobr><b>%2:</b> %3</nobr></div>"
				      "<div><nobr><b>%4:</b> %5</nobr></div>"
				      "<div><nobr><b>%6:</b> %7</nobr></div>"
				      "<div><nobr><b>%8:</b> %9</nobr></div>")
				.arg(t->artist)
				.arg(tr("Country"))
				.arg(t->country)
				.arg(tr("Genre"))
				.arg(t->genre)
				.arg(tr("Language"))
				.arg(t->lang)
				.arg(tr("Last marked as played"))
				.arg(t->lastPlayed);
		return ret;
	}

	return QompPluginTreeModel::data(index, role);
}

QString CuteRadioTune::toString() const
{
	return QString("%1 [%2, %3]").arg(title, genre, country);
}
