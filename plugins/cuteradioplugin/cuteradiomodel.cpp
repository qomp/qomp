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
#include "tune.h"

//--------------------------------------//
//------------CuteRadioModel------------//
//--------------------------------------//
CuteRadioModel::CuteRadioModel(QObject *parent) :
	QompPluginTreeModel(parent)
{
}



//--------------------------------------//
//-------------CuteRadioTune------------//
//--------------------------------------//
QString CuteRadioTune::toString() const
{
	return QString("%1 [%2, %3]").arg(title, genre, country);
}

QString CuteRadioTune::description() const
{
	QString descr("");

	if(!artist.isEmpty())
		descr += QString("<div>%1</div>").arg(artist);
	if(!title.isEmpty())
		descr += makeTooltipString(QObject::tr("Title"), title);
	if(!country.isEmpty())
		descr += makeTooltipString(QObject::tr("Country"), country);
	if(!genre.isEmpty())
		descr += makeTooltipString(QObject::tr("Genre"), genre);
	if(!lang.isEmpty())
		descr += makeTooltipString(QObject::tr("Language"), lang);
	if(!lastPlayed.isEmpty())
		descr += makeTooltipString(QObject::tr("Last marked as played"), lastPlayed);

	return descr;
}

Tune *CuteRadioTune::toTune() const
{
	Tune* t = new Tune(false);
	t->description = artist;
	t->title = title;
	t->url = url;
	return t;
}
