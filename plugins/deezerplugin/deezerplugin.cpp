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

#include "deezerplugin.h"
#include "deezergettunesdlg.h"
#include "deezerplugindefines.h"
#include "qomppluginaction.h"

#include <QtPlugin>
#include <QIcon>

DeezerPlugin::DeezerPlugin() :
	QObject()
{
}

QString DeezerPlugin::name() const
{
	return DEEZER_PLUGIN_NAME;
}

QString DeezerPlugin::version() const
{
	return DEEZER_PLUGIN_VER;
}

QString DeezerPlugin::description() const
{
	return tr("Listen to the music from deezer.com");
}

QList<QompPluginAction *> DeezerPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;
	QompPluginAction *act = new QompPluginAction(QIcon(), tr("Deezer.com"), this, "getTunes", this);
	l.append(act);
	return l;
}

QList<Tune*> DeezerPlugin::getTunes()
{
	QList<Tune*> list;
	DeezerGettunesDlg dlg;
	if(dlg.go() == QompPluginGettunesDlg::ResultOK) {
		list = dlg.getTunes();
	}
	return list;
}


QompOptionsPage *DeezerPlugin::options()
{
	return 0;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(deezerplugin, DeezerPlugin)
#endif
