/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#ifndef NOTIFICATIONSPLUGIN_H
#define NOTIFICATIONSPLUGIN_H

#include "qompplugin.h"
#include "qompplayerstatusplugin.h"
#include "qompplayer.h"


class NotificationsPlugin : public QObject, public QompPlugin, public QompPlayerStatusPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompPlayerStatusPlugin)
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")

public:
	NotificationsPlugin();
	~NotificationsPlugin();

	virtual QString name() const Q_DECL_FINAL { return tr("Notifications"); }
	virtual QString version() const Q_DECL_FINAL { return "1.4"; }
	virtual QString description() const Q_DECL_FINAL { return tr("Shows baloon notifications about current track"); }
	virtual QompOptionsPage* options() Q_DECL_FINAL;
	virtual void qompPlayerChanged(QompPlayer* player) Q_DECL_FINAL;
	virtual void playerControlChanged(QompPlayerControl* control) Q_DECL_FINAL { Q_UNUSED(control) }
	virtual void setEnabled(bool enabled) Q_DECL_FINAL;
	virtual void unload() Q_DECL_FINAL;

private slots:
	void playerStatusChanged(Qomp::State state);
	void init();

private:
	QompPlayer *player_;
	bool enabled_;
	class Private;
	Private* d;
	friend class Private;
};

#endif // NOTIFICATIONSPLUGIN_H
