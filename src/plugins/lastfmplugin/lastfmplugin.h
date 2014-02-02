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

#ifndef LASTFMPLUGIN_H
#define LASTFMPLUGIN_H

#include "qompplugin.h"
#include "qompplayerstatusplugin.h"
#include "lastfmdefines.h"

#include <QPointer>

#ifndef QT_STATICPLUGIN
#define QT_STATICPLUGIN
#endif

class Tune;
class QNetworkAccessManager;
class LastFmSettings;
class QTimer;

class LastFmPlugin : public QObject, public QompPlugin, public QompPlayerStatusPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin QompPlayerStatusPlugin)
#ifdef HAVE_QT5
	Q_PLUGIN_METADATA(IID "Qomp.QompPlugin/0.1")
	Q_PLUGIN_METADATA(IID "Qomp.QompPlayerStatusPlugin/0.1")
#endif
public:
	explicit LastFmPlugin();
	~LastFmPlugin() {}
	virtual QString name() const { return LASTFM_NAME; }
	virtual QString version() const { return LASTFM_VER; }
	virtual QString description() const { return tr("LastFm scrobbling"); }
	virtual QompOptionsPage* options();
	virtual void qompPlayerChanged(QompPlayer* player);
	virtual void setEnabled(bool enabled);
	virtual void unload() {}

private slots:
	void playerStatusChanged();
	void tuneChanged(Tune* t);
	void login();
	void loginStepTwo();
	void loginStepThree();
	void init();
	void postFinished();
	void updateNowPlaying();
	void scrobble();

private:
	QompPlayer *player_;
	QNetworkAccessManager* nam_;
	QPointer<LastFmSettings> settings_;
	Tune* currentTune_;
	QTimer* scrobbleTimer_;
	QTimer* nowPlayingTimer_;
	bool enabled_;
};

#endif // LASTFMPLUGIN_H
