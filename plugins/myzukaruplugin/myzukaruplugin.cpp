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

#include "myzukaruplugin.h"
#include "myzukarudefines.h"
#include "myzukarucontroller.h"
#include "myzukaruresolvestrategy.h"

#include <QtPlugin>

MyzukaruPlugin::MyzukaruPlugin()
{
}

QString MyzukaruPlugin::name() const
{
	return MYZUKA_PLUGIN_NAME;
}

QString MyzukaruPlugin::version() const
{
	return MYZUKA_PLUGIN_VERSION;
}

QString MyzukaruPlugin::description() const
{
	return tr("Listen to the music from Myzuka.ru");
}

QList<Tune*> MyzukaruPlugin::getTunes()
{
	MyzukaruController ctrl;
	return ctrl.getTunes();
}


QompOptionsPage *MyzukaruPlugin::options()
{
	return 0;
}

TuneURLResolveStrategy *MyzukaruPlugin::urlResolveStrategy() const
{
	return MyzukaruResolveStrategy::instance();
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(myzukaruplugin, MyzukaruPlugin)
#endif
