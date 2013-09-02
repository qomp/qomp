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

#ifndef MPRISPLUGIN_H
#define MPRISPLUGIN_H

#ifndef QT_STATICPLUGIN
#define QT_STATICPLUGIN
#endif

#include "qompplugin.h"
#include "qompplayerstatusplugin.h"

class MprisController;

class MprisPlugin : public QObject, public QompPlugin, public QompPlayerStatusPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompPlayerStatusPlugin)
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1")
	Q_PLUGIN_METADATA(IID "Qomp.QompPlayerStatusPlugin/0.1")
#endif
public:
	explicit MprisPlugin();
	~MprisPlugin() {}
	virtual QString name() const { return tr("MPRIS Plugin"); }
	virtual QString version() const { return "0.1"; }
	virtual QString description() const { return tr("Interface to MPRIS"); }
	virtual QompOptionsPage* options() { return 0; }
	virtual void qompPlayerChanged(QompPlayer* player);
	virtual void setEnabled(bool enabled);
	virtual void unload();

private slots:
	void playerStatusChanged();

private:
	void disableMpris();

private:
	QompPlayer *player_;
	bool enabled_;
	MprisController* mpris_;
};

#endif // MPRISPLUGIN_H
