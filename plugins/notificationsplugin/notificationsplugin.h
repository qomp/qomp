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
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")
#endif

public:
	explicit NotificationsPlugin();
	~NotificationsPlugin();

	virtual QString name() const { return tr("Notifications"); }
	virtual QString version() const { return "1.1.1"; }
	virtual QString description() const { return tr("Shows baloon notifications about current track"); }
	virtual QompOptionsPage* options();
	virtual void qompPlayerChanged(QompPlayer* player);
	virtual void setEnabled(bool enabled);
	virtual void unload();

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
