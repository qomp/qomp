/*
 * Copyright (C) 2018  Khryukin Evgeny
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

#include "poiskmplugin.h"
#include "poiskmcontroller.h"
#include "qomppluginaction.h"
#include "tune.h"
#include "poiskmurlresolvestrategy.h"

#include <QtPlugin>


PoiskmPlugin::PoiskmPlugin() : enabled_(false)
{
}

QString PoiskmPlugin::description() const
{
	return tr("Listen to the music from Poiskm.me");
}

void PoiskmPlugin::getTunes(QompPluginAction *act)
{
	PoiskmController* ctrl = new PoiskmController(this);
	connect(ctrl, &PoiskmController::tunesReady, act, &QompPluginAction::setTunesReady);
	ctrl->getTunes();
}

QompOptionsPage *PoiskmPlugin::options()
{
	return nullptr;
}

TuneURLResolveStrategy *PoiskmPlugin::urlResolveStrategy() const
{
	return PoiskmURLResolveStrategy::instance();
}

void PoiskmPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void PoiskmPlugin::unload()
{
	PoiskmURLResolveStrategy::instance()->reset();
}

QList<QompPluginAction *> PoiskmPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;
	QompPluginAction *act = new QompPluginAction(QIcon(), tr("Poiskm"), this, "getTunes", this);
	l.append(act);
	return l;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(PoiskmPlugin, PoiskmPlugin)
#endif
