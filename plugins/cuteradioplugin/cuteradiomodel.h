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

#ifndef CUTERADIOMODEL_H
#define CUTERADIOMODEL_H

#include "qompplugintreemodel.h"
#include "qompplugintypes.h"

class CuteRadioTune : public QompPluginTune
{
public:
	QString lastPlayed;
	QString country;
	QString lang;
	QString genre;

	virtual QString toString() const Q_DECL_OVERRIDE;
	virtual QString description() const Q_DECL_OVERRIDE;
	virtual Tune* toTune() const Q_DECL_OVERRIDE;
};



class CuteRadioModel : public QompPluginTreeModel
{
public:
	explicit CuteRadioModel(QObject *parent = 0);
};

#endif // CUTERADIOMODEL_H
