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

#include "qompplugin.h"
#include "qompplayerstatusplugin.h"
#include "mpriscontroller.h"
#include "tune.h"
#include "common.h"
#include <QTemporaryFile>

class MprisPlugin : public QObject, public QompPlugin, public QompPlayerStatusPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompPlayerStatusPlugin)
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1" FILE "metadata.json")
public:
	MprisPlugin();
	~MprisPlugin() {}
	virtual QString name() const Q_DECL_FINAL { return tr("MPRIS Plugin"); }
	virtual QString version() const Q_DECL_FINAL { return "1.4"; }
	virtual QString description() const Q_DECL_FINAL { return tr("Interface to MPRIS"); }
	virtual QompOptionsPage* options() Q_DECL_FINAL { return 0; }
	virtual void qompPlayerChanged(QompPlayer* player) Q_DECL_FINAL;
	virtual void playerControlChanged(QompPlayerControl* control) Q_DECL_FINAL;
	virtual void setEnabled(bool enabled) Q_DECL_FINAL;
	virtual void unload() Q_DECL_FINAL;

private slots:
	void playerStatusChanged(Qomp::State state);
	void play();
	void pause();
	void stop();
	void next();
	void previous();
	void setVolume(const qreal &volume);
	void doQuit();
	void doRaise();
	void updateVolume();
	void updatePosition();
	void setPosition(const qreal &position);
	void tuneUpdated(Tune *tune);

private:
	void disableMpris();
	void getMetaData(Tune *tune);
	void sendMetadata(const QString &status);
	QString getAlbumArtFile(const QImage &art);

private:
	QompPlayer *player_;
	bool enabled_;
	MprisController* mpris_;
	QompMetaData *tune_;
	Tune *lastTune_;
	QTemporaryFile *artFile_;
};

#endif // MPRISPLUGIN_H
