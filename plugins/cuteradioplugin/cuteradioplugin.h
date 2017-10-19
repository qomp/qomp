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

#ifndef CUTERADIOPLUGIN_H
#define CUTERADIOPLUGIN_H

#include "qompplugin.h"
#include "qomptunepluign.h"
#include "cuteradioplugindefines.h"

#include <QPointer>

class CuteRadioPlugin : public QObject, public QompPlugin, public QompTunePlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompTunePlugin)
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")
#endif
public:
	CuteRadioPlugin();
	virtual QString name() const { return CUTERADIO_PLUGIN_NAME; }
	virtual QString version() const { return CUTERADIO_PLUGIN_VERSION; }
	virtual QString description() const;
	virtual QompOptionsPage* options();
	virtual TuneURLResolveStrategy* urlResolveStrategy() const { return nullptr; }
	virtual void setEnabled(bool enabled);
	virtual void unload();
	virtual QList<QompPluginAction*> getTunesActions();
	virtual bool processUrl(const QString &, QList<Tune*> *) { return false; }

private slots:
	void getTunes(QompPluginAction *act);

private:
	bool enabled_;
};

#endif // CUTERADIOPLUGIN_H
