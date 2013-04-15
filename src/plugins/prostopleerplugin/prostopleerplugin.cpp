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

#include "prostopleerplugin.h"
#include "prostopleerplugingettunesdialog.h"
#include "prostopleerplugindefines.h"
#include "prostopleerpluginsettings.h"

#include <QtPlugin>


ProstoPleerPlugin::ProstoPleerPlugin()
{
}

QString ProstoPleerPlugin::name() const
{
	return PROSTOPLEER_PLUGIN_NAME;
}

QString ProstoPleerPlugin::version() const
{
	return PROSTOPLEER_PLUGIN_VERSION;
}

QString ProstoPleerPlugin::description() const
{
	return QString();
}

TuneList ProstoPleerPlugin::getTunes()
{
	TuneList list;
	ProstoPleerPluginGetTunesDialog gtd;
	if(gtd.exec() == QDialog::Accepted)
		list = gtd.getTunes();
	return list;
}

QompOptionsPage *ProstoPleerPlugin::options()
{
	return new ProstopleerPluginSettings();
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(prostopleerplugin, ProstoPleerPlugin)
#endif
