/*
 * Copyright (C) 2013-2015  Khryukin Evgeny
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

#ifndef TUNETOFILEPLUGIN_H
#define TUNETOFILEPLUGIN_H

#include "qompplugin.h"
#include "qompplayerstatusplugin.h"
#include "qompplayer.h"

#include <QPointer>

class TuneToFileSettings;

class TuneToFilePlugin : public QObject, public QompPlugin, public QompPlayerStatusPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompPlayerStatusPlugin)
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")

public:
	TuneToFilePlugin();

	virtual QString name() const Q_DECL_FINAL { return tr("Tune to File"); }
	virtual QString version() const Q_DECL_FINAL { return "1.5"; }
	virtual QString description() const Q_DECL_FINAL { return tr("Store current tune into file. Usefull for publishing tune in status"); }
	virtual QompOptionsPage* options() Q_DECL_FINAL;
	virtual void qompPlayerChanged(QompPlayer* player) Q_DECL_FINAL;
	virtual void playerControlChanged(QompPlayerControl* control) Q_DECL_FINAL { Q_UNUSED(control) }
	virtual void setEnabled(bool enabled) Q_DECL_FINAL;
	virtual void unload() Q_DECL_FINAL;

private slots:
	void playerStatusChanged(Qomp::State state);
	void optionsUpdate();
	void init();

private:
	QompPlayer *player_;
	QString file_;
	bool enabled_;
	QPointer<TuneToFileSettings> optPage_;
};

#endif // TUNETOFILEPLUGIN_H
