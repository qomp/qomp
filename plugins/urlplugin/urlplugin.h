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

#ifndef URLPLUGIN_H
#define URLPLUGIN_H

#include "qompplugin.h"
#include "qomptunepluign.h"

#define PLUGIN_NAME "Url"

class UrlPlugin : public QObject, public QompPlugin, public QompTunePlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompTunePlugin)
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")
public:
	UrlPlugin();
	virtual QString name() const Q_DECL_FINAL { return PLUGIN_NAME; }
	virtual QString version() const Q_DECL_FINAL { return "1.4"; }
	virtual QString description() const Q_DECL_FINAL { return tr("Open custom URL"); }
	virtual QompOptionsPage* options() Q_DECL_FINAL;
	virtual TuneURLResolveStrategy* urlResolveStrategy() const Q_DECL_FINAL { return 0; }
	virtual void setEnabled(bool /*enabled*/) Q_DECL_FINAL {}
	virtual void unload() Q_DECL_FINAL {}
	virtual QList<QompPluginAction*> getTunesActions() Q_DECL_FINAL;
	virtual bool processUrl(const QString &, QList<Tune*> *) Q_DECL_FINAL;

private slots:
	void getTunes(QompPluginAction* act);

private:
	class Private;
	Private* d;
};

#endif // URLPLUGIN_H
