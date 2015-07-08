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
#include "prostopleercontroller.h"
#include "prostopleerpluginsettings.h"
#include "qomppluginaction.h"

#include <QtPlugin>


ProstoPleerPlugin::ProstoPleerPlugin() : enabled_(false)
{
}

QString ProstoPleerPlugin::description() const
{
	return tr("Listen to the music from Pleer.com");
}

QList<Tune*> ProstoPleerPlugin::getTunes()
{
	ProstoPleerController ctrl;
	return ctrl.getTunes();
}

QompOptionsPage *ProstoPleerPlugin::options()
{
	if(!enabled_)
		return 0;

	optPage_ = new ProstopleerPluginSettings();
	return optPage_ ;
}

void ProstoPleerPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void ProstoPleerPlugin::unload()
{
	if(optPage_)
		delete optPage_;
}

QList<QompPluginAction *> ProstoPleerPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;
	QompPluginAction *act = new QompPluginAction(QIcon(), tr("Pleer.com"), this, "getTunes", this);
	l.append(act);
	return l;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(prostopleerplugin, ProstoPleerPlugin)
#endif
