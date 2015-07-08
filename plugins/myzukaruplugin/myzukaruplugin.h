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

#ifndef MYZUKARUPLUGIN_H
#define MYZUKARUPLUGIN_H

#include "qompplugin.h"
#include "qomptunepluign.h"
#include "myzukarudefines.h"

class MyzukaruPlugin : public QObject, public QompPlugin, public QompTunePlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompTunePlugin)
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")
#endif
public:
	MyzukaruPlugin();
	virtual QString name() const { return MYZUKA_PLUGIN_NAME; }
	virtual QString version() const { return MYZUKA_PLUGIN_VERSION; }
	virtual QString description() const;
	virtual QompOptionsPage* options();
	virtual TuneURLResolveStrategy* urlResolveStrategy() const;
	virtual void setEnabled(bool /*enabled*/) {}
	virtual void unload();
	virtual QList<QompPluginAction*> getTunesActions();

private slots:
	QList<Tune*> getTunes();
};

#endif // MYZUKARUPLUGIN_H
