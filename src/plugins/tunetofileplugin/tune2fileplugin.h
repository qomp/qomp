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

#ifndef TUNE2FILEPLUGIN_H
#define TUNE2FILEPLUGIN_H

#include "qompplugin.h"
#include "qompplayerstatusplugin.h"
#include "qompplayer.h"

#ifndef QT_STATICPLUGIN
#define QT_STATICPLUGIN
#endif


class Tune2FilePlugin : public QObject, public QompPlugin, public QompPlayerStatusPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompPlayerStatusPlugin)
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1")
	Q_PLUGIN_METADATA(IID "Qomp.QompPlayerStatusPlugin/0.1")
#endif

public:
	explicit Tune2FilePlugin();

	virtual QString name() const { return tr("Tune to File Plugin"); }
	virtual QString version() const { return "0.1"; }
	virtual QString description() const { return tr("Store current tune into file. Usefull for publishing tune in status"); }
	virtual QompOptionsPage* options();
	virtual void qompPlayerChanged(QompPlayer* player);
	virtual void setEnabled(bool enabled);
	virtual void unload();

private slots:
	void playerStatusChanged( QompPlayer::State state);
	void optionsUpdate();

private:
	QompPlayer *player_;
	QString file_;
	bool enabled_;
};

#endif // TUNE2FILEPLUGIN_H
