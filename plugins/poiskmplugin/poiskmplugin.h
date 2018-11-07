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

#ifndef POISKMPLUGIN_H
#define POISKMPLUGIN_H

#include "qompplugin.h"
#include "qomptunepluign.h"
#include "poiskmplugindefines.h"

class PoiskmPlugin : public QObject, public QompPlugin, public QompTunePlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompTunePlugin)
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")
#endif
public:
	PoiskmPlugin();
	virtual QString name() const Q_DECL_OVERRIDE { return POISKM_PLUGIN_NAME; }
	virtual QString version() const  Q_DECL_OVERRIDE { return POISKM_PLUGIN_VERSION; }
	virtual QString description() const Q_DECL_OVERRIDE;
	virtual QompOptionsPage* options() Q_DECL_OVERRIDE;
	virtual TuneURLResolveStrategy* urlResolveStrategy() const Q_DECL_OVERRIDE;
	virtual void setEnabled(bool enabled) Q_DECL_OVERRIDE;
	virtual void unload() Q_DECL_OVERRIDE;
	virtual QList<QompPluginAction*> getTunesActions() Q_DECL_OVERRIDE;
	virtual bool processUrl(const QString &, QList<Tune*> *)  Q_DECL_OVERRIDE { return false; }

private slots:
	void getTunes(QompPluginAction* act);

private:
	bool enabled_;
};

#endif // POISKMPLUGIN_H
