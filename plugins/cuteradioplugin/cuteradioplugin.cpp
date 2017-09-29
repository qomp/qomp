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

#include "cuteradioplugin.h"
#include "cuteradiocontroller.h"
#include "qomppluginaction.h"

#include <QtPlugin>


CuteRadioPlugin::CuteRadioPlugin() : enabled_(false)
{
}

QString CuteRadioPlugin::description() const
{
	return tr("Listen to the music from cuteRadio");
}

void CuteRadioPlugin::getTunes(QompPluginAction *action)
{
	CuteRadioController ctrl;
	action->setTunesReady( ctrl.getTunes() );
}

QompOptionsPage *CuteRadioPlugin::options()
{
	return nullptr;
}

void CuteRadioPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void CuteRadioPlugin::unload()
{
}

QList<QompPluginAction *> CuteRadioPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;
	QompPluginAction *act = new QompPluginAction(QIcon(), tr("cuteRadio"), this, "getTunes", this);
	l.append(act);
	return l;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(cuteradioplugin, CuteRadioPlugin)
#endif
