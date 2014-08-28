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

#include "qompcon.h"
#include "qompmainwin.h"
#include "options.h"
#include "defines.h"
#include "qompnetworkingfactory.h"
#include "translator.h"
#include "qompplaylistmodel.h"
#include "qompoptionsdlg.h"
#include "pluginmanager.h"
#include "qompplayer.h"
#include "tune.h"
#ifndef Q_OS_ANDROID
#include "aboutdlg.h"
#include "thememanager.h"
#include "updateschecker.h"
#include "qomptunedownloader.h"
#else
#include "qompqmlengine.h"
#endif


#include <QCoreApplication>
#ifdef HAVE_QT5
#include <QThread>
#endif

#ifdef HAVE_PHONON
#include "qompphononplayer.h"
#elif HAVE_QTMULTIMEDIA
#include "qompqtmultimediaplayer.h"
#endif

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

QompCon::QompCon(QObject *parent) :
	QObject(parent),
	mainWin_(0),
	model_(0),
	player_(0)
{
	qRegisterMetaType<Tune*>("Tune*");
	qRegisterMetaType<Qomp::State>("State");
#ifdef Q_OS_ANDROID
	connect(QompQmlEngine::instance(), SIGNAL(quit()), SLOT(exit()));
#endif
}

QompCon::~QompCon()
{
	model_->saveState();

	Options::instance()->setOption(OPTION_VOLUME, player_->volume());

	delete mainWin_;
	delete player_;
	delete Tune::emptyTune();
#ifdef Q_OS_ANDROID
	delete QompQmlEngine::instance();
#endif
}

void QompCon::checkVersion()
{
	QVariant vVer = Options::instance()->getOption(OPTION_APPLICATION_VERSION);
	if(vVer == QVariant::Invalid  //First launch
			|| vVer.toString() != APPLICATION_VERSION) {
		QHash <const char*, QVariant> hash;
		hash.insert(OPTION_START_MINIMIZED,	false);
		hash.insert(OPTION_AUTOSTART_PLAYBACK,	false);
		hash.insert(OPTION_UPDATE_METADATA,	false);
		hash.insert(OPTION_PROXY_USE,		false);
		hash.insert(OPTION_PROXY_TYPE,		"HTTP");
		hash.insert(OPTION_PROXY_PORT,		"3128");
		hash.insert(OPTION_HIDE_ON_CLOSE,	true);
		hash.insert(OPTION_DEFAULT_ENCODING,	"CP1251");
		hash.insert(OPTION_TRAY_DOUBLE_CLICK,	1);
		hash.insert(OPTION_TRAY_MIDDLE_CLICK,	0);
		hash.insert(OPTION_TRAY_LEFT_CLICK,	2);
		hash.insert(OPTION_REPEAT_ALL,		false);
		hash.insert(OPTION_PLUGINS_ORDER,	QStringList()	<< "Myzuka.ru"
									<< "Yandex.Music"
									<< "Pleer.com");
		hash.insert(OPTION_THEME,		"blue");
		hash.insert(OPTION_CURRENT_TRANSLATION, QLocale::system().name().split("_").first());

		foreach(const char* key, hash.keys()) {
			if(Options::instance()->getOption(key) == QVariant::Invalid)
				Options::instance()->setOption(key, hash.value(key));
		}

		Options::instance()->setOption(OPTION_APPLICATION_VERSION, APPLICATION_VERSION);
	}
}

void QompCon::init()
{
	checkVersion();
	updateSettings();

	setupModel();
	setupPlayer();
	setupMainWin();

	model_->restoreState();

	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(updateSettings()));

	if(Options::instance()->getOption(OPTION_START_MINIMIZED).toBool())
		mainWin_->hide();
	else
		mainWin_->show();

	if(Options::instance()->getOption(OPTION_AUTOSTART_PLAYBACK).toBool())
		actPlay();
}

void QompCon::exit()
{
	qApp->exit();
}

void QompCon::updateSettings()
{
	QompNetworkingFactory::instance()->updateProxySettings();
#ifndef Q_OS_ANDROID
	ThemeManager::instance()->setTheme(Options::instance()->getOption(OPTION_THEME).toString());
#endif
	Translator::instance()->retranslate(Options::instance()->getOption(OPTION_CURRENT_TRANSLATION).toString());
}

void QompCon::actPlayNext()
{
	QModelIndex index = model_->currentIndex();
	if(index.isValid() && index.row() < model_->rowCount()-1) {
		bool play = (player_->state() == Qomp::StatePlaying);
		index = model_->index(index.row()+1);
		model_->setCurrentTune(model_->tune(index));
		if(play) {
			stopPlayer();
			actPlay();
		}
	}
}

void QompCon::actPlayPrev()
{
	QModelIndex index = model_->currentIndex();
	if(index.isValid() && index.row() > 0) {
		bool play = (player_->state() == Qomp::StatePlaying);
		index = model_->index(index.row()-1);
		model_->setCurrentTune(model_->tune(index));
		if(play) {
			stopPlayer();
			actPlay();
		}
	}
}

void QompCon::actPlay()
{
	if(player_->state() == Qomp::StatePlaying) {
		actPause();
		return;
	}

	if(!model_->rowCount())
		return;

	QModelIndex i = model_->currentIndex();
	if(!i.isValid()) {
		i = model_->index(0);
		model_->setCurrentTune(model_->tune(i));
	}

	player_->play();
}

void QompCon::actPause()
{
	player_->pause();
}

void QompCon::actStop()
{
	stopPlayer();
}

void QompCon::actMediaActivated(const QModelIndex &index)
{
	playIndex(index);
}

void QompCon::actMediaClicked(const QModelIndex &index)
{
	Q_UNUSED(index)
//	if(player_->state() == Qomp::StateStopped) {
//		model_->setCurrentTune(model_->tune(index));
//	}
}

void QompCon::actMuteToggle(bool mute)
{
	if(player_->isMuted() != mute) {
		player_->setMute(mute);
		mainWin_->setMuteState(player_->isMuted());
	}
}

void QompCon::actSeek(int ms)
{
	player_->setPosition(qint64(ms));
}

void QompCon::actSetVolume(qreal vol)
{
	player_->setVolume(vol);
}

void QompCon::setTunes(const QList<Tune*> &tunes)
{
	if(!tunes.isEmpty()) {
		model_->addTunes(tunes);

		if(player_->state() != Qomp::StatePaused
			&& player_->state() != Qomp::StatePlaying)
		{
			QModelIndex index = model_->indexForTune(tunes.first());
			model_->setCurrentTune(model_->tune(index));
			actPlay();
		}

//		if(Options::instance()->getOption(OPTION_UPDATE_METADATA).toBool())
//		{
//			player_->resolveMetadata(tunes);
//		}
	}
}

void QompCon::actClearPlaylist()
{
	stopPlayer();
	model_->clear();
}

void QompCon::actRemoveSelected(const QModelIndexList &list)
{
	QList<Tune*> tl;
	foreach(const QModelIndex& index, list)
		tl.append(model_->tune(index));

	foreach(Tune* tune, tl)
		model_->removeTune(tune);
}

void QompCon::actDoSettings()
{
	QompOptionsDlg dlg(player_);
	dlg.exec();
}

void QompCon::actCheckForUpdates()
{
#ifndef Q_OS_ANDROID
	new UpdatesChecker(this);
#endif
}

void QompCon::actAboutQomp()
{
#ifndef Q_OS_ANDROID
	new AboutDlg(0);
#endif
}

void QompCon::actDownloadTune(Tune *tune, const QString &dir)
{
#ifndef Q_OS_ANDROID
	QompTuneDownloader *td = new QompTuneDownloader(this);
	td->download(tune, dir);
#endif
}

void QompCon::actToggleTuneState(Tune *tune)
{
	model_->setCurrentTune(tune);
	actPlay();
}

void QompCon::actRemoveTune(Tune *tune)
{
	if(model_->currentTune() == tune) {
		model_->setCurrentTune(model_->tune(model_->index(0,0)));
	}
	model_->removeTune(tune);
}

void QompCon::setupMainWin()
{
	mainWin_ = new QompMainWin();
	mainWin_->setModel(model_);

	mainWin_->setMuteState(player_->isMuted());
	mainWin_->volumeChanged(player_->volume());
	mainWin_->setCurrentPosition(player_->position());
	mainWin_->playerStateChanged(player_->state());

	connect(player_, SIGNAL(currentPositionChanged(qint64)),	mainWin_, SLOT(setCurrentPosition(qint64)));
	connect(player_, SIGNAL(mutedChanged(bool)),			mainWin_, SLOT(setMuteState(bool)));
	connect(player_, SIGNAL(volumeChanged(qreal)),			mainWin_, SLOT(volumeChanged(qreal)));
	connect(player_, SIGNAL(currentTuneTotalTimeChanged(qint64)),	mainWin_, SLOT(currentTotalTimeChanged(qint64)));

	connect(mainWin_, SIGNAL(exit()),				SLOT(exit()));
	connect(mainWin_, SIGNAL(aboutQomp()),				SLOT(actAboutQomp()));
	connect(mainWin_, SIGNAL(checkForUpdates()),			SLOT(actCheckForUpdates()));
	connect(mainWin_, SIGNAL(doOptions()),				SLOT(actDoSettings()));
	connect(mainWin_, SIGNAL(downloadTune(Tune*,QString)),		SLOT(actDownloadTune(Tune*,QString)));
	connect(mainWin_, SIGNAL(actMuteActivated(bool)),		SLOT(actMuteToggle(bool)));
	connect(mainWin_, SIGNAL(volumeSliderMoved(qreal)),		SLOT(actSetVolume(qreal)));
	connect(mainWin_, SIGNAL(seekSliderMoved(int)),			SLOT(actSeek(int)));
	connect(mainWin_, SIGNAL(removeSelected(QModelIndexList)),	SLOT(actRemoveSelected(QModelIndexList)));
	connect(mainWin_, SIGNAL(clearPlaylist()),			SLOT(actClearPlaylist()));
	connect(mainWin_, SIGNAL(actPlayActivated()),			SLOT(actPlay()));
	connect(mainWin_, SIGNAL(actNextActivated()),			SLOT(actPlayNext()));
	connect(mainWin_, SIGNAL(actPrevActivated()),			SLOT(actPlayPrev()));
	connect(mainWin_, SIGNAL(actStopActivated()),			SLOT(actStop()));
	connect(mainWin_, SIGNAL(tunes(QList<Tune*>)),			SLOT(setTunes(QList<Tune*>)));
	connect(mainWin_, SIGNAL(toggleTuneState(Tune*)),		SLOT(actToggleTuneState(Tune*)));
	connect(mainWin_, SIGNAL(removeTune(Tune*)),			SLOT(actRemoveTune(Tune*)));
	connect(mainWin_, SIGNAL(mediaActivated(QModelIndex)),		SLOT(actMediaActivated(QModelIndex)));
	connect(mainWin_, SIGNAL(mediaClicked(QModelIndex)),		SLOT(actMediaClicked(QModelIndex)));
}

void QompCon::setupPlayer()
{
	player_ = createPlayer();
	PluginManager::instance()->qompPlayerChanged(player_);

	connect(player_, SIGNAL(tuneDataUpdated(Tune*)), model_, SLOT(tuneDataUpdated(Tune*)));
	connect(player_, SIGNAL(mediaFinished()), SLOT(mediaFinished()));
	connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStateChanged(Qomp::State)));
	connect(model_,  SIGNAL(currentTuneChanged(Tune*)), player_, SLOT(setTune(Tune*)));

#ifndef Q_OS_ANDROID
	player_->setVolume(Options::instance()->getOption(OPTION_VOLUME, 1).toReal());
	player_->setAudioOutputDevice(Options::instance()->getOption(OPTION_AUDIO_DEVICE).toString());
#endif
}

void QompCon::setupModel()
{
	model_ = new QompPlayListModel(this);
}

void QompCon::stopPlayer()
{
	player_->blockSignals(true);
	player_->stop();
	mainWin_->setCurrentPosition(0);
	playerStateChanged(Qomp::StateStopped);
	while (player_->state() != Qomp::StateStopped) {
#ifdef HAVE_QT5
		QThread::sleep(1);
#endif
		qApp->processEvents();
	}
	player_->blockSignals(false);
}

void QompCon::playIndex(const QModelIndex &index)
{
	stopPlayer();
	model_->setCurrentTune(model_->tune(index));
	actPlay();
}

QompPlayer *QompCon::createPlayer()
{
#ifdef HAVE_PHONON
	return new QompPhononPlayer;
#elif HAVE_QTMULTIMEDIA
	return new QompQtMultimediaPlayer;
#endif
}

void QompCon::mediaFinished(bool afterError)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompCon::mediaFinished()";
#endif
	mainWin_->setCurrentPosition(0);

	QModelIndex index = model_->currentIndex();

	//If current tune is empty tune, we probably reachs the end of playlist
	if(!index.isValid())
		return;

	if(index.row() == model_->rowCount()-1) {
		if(!afterError && Options::instance()->getOption(OPTION_REPEAT_ALL).toBool()) {
			const QModelIndex ind = model_->index(0);
			playIndex(ind);

		}
		else {
			actStop();
			model_->setCurrentTune((Tune*)Tune::emptyTune());
		}
	}
	else {
		index = model_->index(index.row()+1);
		playIndex(index);
	}
}

void QompCon::playerStateChanged(Qomp::State state)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompCon::playerStateChanged()  " << state;
#endif
	mainWin_->playerStateChanged(state);
	if (state == Qomp::StateError && QompNetworkingFactory::instance()->isNetworkAvailable())
		mediaFinished(true);
}
