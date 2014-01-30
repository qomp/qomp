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

#include "qomp.h"
#include "qompmainwin.h"
#include "options.h"
#include "defines.h"
#include "qompnetworkingfactory.h"
#include "translator.h"
#ifdef HAVE_PHONON
#include "qompphononplayer.h"
#elif HAVE_QTMULTIMEDIA
#include "qompqtmultimediaplayer.h"
#endif
#include "updateschecker.h"
#include "aboutdlg.h"
#include "qomptunedownloader.h"
#include "qompplaylistmodel.h"
#include "qompoptionsdlg.h"

#include <QApplication>
#include <QFileDialog>

Qomp::Qomp(QObject *parent) :
	QObject(parent),
	mainWin_(0),
	model_(0),
	player_(0)
{
	qRegisterMetaType<Tune*>("Tune*");

	Translator::instance();

	setupModel();
	setupPlayer();
	setupMainWin();

	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(updateOptions()));

	if(Options::instance()->getOption(OPTION_START_MINIMIZED).toBool())
		mainWin_->hide();
	else
		mainWin_->show();

	if(Options::instance()->getOption(OPTION_AUTOSTART_PLAYBACK).toBool())
		mainWin_->actPlayActivated();
}

Qomp::~Qomp()
{
	model_->saveState();

	delete mainWin_;
	delete Tune::emptyTune();
}

void Qomp::init()
{
	QVariant vVer = Options::instance()->getOption(OPTION_APPLICATION_VERSION);
	if(vVer == QVariant::Invalid) { //First launch
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

		foreach(const char* key, hash.keys()) {
			Options::instance()->setOption(key, hash.value(key));
		}
	}
	QString ver = vVer.toString();
	if(ver != APPLICATION_VERSION) {
		//Here in the future we can do some updates
		if(Options::instance()->getOption(OPTION_REPEAT_ALL) == QVariant::Invalid) {
			Options::instance()->setOption(OPTION_REPEAT_ALL, false);
		}
		Options::instance()->setOption(OPTION_APPLICATION_VERSION, APPLICATION_VERSION);
	}
	updateOptions();
}

void Qomp::exit()
{
	qApp->exit();
}

void Qomp::updateOptions()
{
	QompNetworkingFactory::instance()->updateProxySettings();
	if(mainWin_->player())
		mainWin_->player()->setAudioOutputDevice(Options::instance()->getOption(OPTION_AUDIO_DEVICE).toString());
}

void Qomp::actPlayNext()
{

}

void Qomp::actPlayPrev()
{

}

void Qomp::actPlay()
{

}

void Qomp::actPause()
{

}

void Qomp::actStop()
{

}

void Qomp::actMuteToggle()
{

}

void Qomp::actSeek(qint64 ms)
{

}

void Qomp::actSetVolume(qreal vol)
{

}

void Qomp::actSavePlaylist()
{
	QString file = QFileDialog::getSaveFileName(mainWin_, tr("Save Playlist"),
			Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), "*.qomp");
	if(!file.isEmpty()) {
		Options::instance()->setOption(LAST_DIR, file);
		model_->saveTunes(file);
	}
}

void Qomp::actLoadPlaylist()
{
	QString file = QFileDialog::getOpenFileName(mainWin_, tr("Select Playlist"),
			Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), "*.qomp");
	if(!file.isEmpty()) {
		Options::instance()->setOption(LAST_DIR, file);
		model_->loadTunes(file);
	}
}

void Qomp::actGetTunes()
{

}

void Qomp::actClearPlaylist()
{

}

void Qomp::actRemoveSelected(const QModelIndexList &list)
{

}

void Qomp::actDoSettings()
{
	QompOptionsDlg dlg(mainWin_);
	dlg.exec();
}

void Qomp::actCheckForUpdates()
{
	new UpdatesChecker(this);
}

void Qomp::actAboutQomp()
{
	new AboutDlg(mainWin_);
}

void Qomp::actDownloadTune(Tune *tune)
{
	static const QString option = "main.last-save-dir";
	QString dir = QFileDialog::getExistingDirectory(mainWin_, tr("Select directory"),
				Options::instance()->getOption(option, QDir::homePath()).toString());
	if(dir.isEmpty())
		return;

	Options::instance()->setOption(option, dir);
	QompTuneDownloader *td = new QompTuneDownloader(this);
	td->download(tune, dir);
}

void Qomp::actToggleTune(Tune *tune)
{

}

void Qomp::actRemoveTune(Tune *tune)
{

}

void Qomp::setupMainWin()
{
	mainWin_ = new QompMainWin();
	mainWin_->setModel(model_);
	mainWin_->setPlayer(player_);

	connect(mainWin_, SIGNAL(exit()), SLOT(exit()));
	connect(mainWin_, SIGNAL(loadPlaylist()), SLOT(actLoadPlaylist()));
	connect(mainWin_, SIGNAL(savePlaylist()), SLOT(actSavePlaylist()));
	connect(mainWin_, SIGNAL(aboutQomp()), SLOT(actAboutQomp()));
	connect(mainWin_, SIGNAL(checkForUpdates()), SLOT(actCheckForUpdates()));
	connect(mainWin_, SIGNAL(doOptions()), SLOT(actDoSettings()));
	connect(mainWin_, SIGNAL(downloadTune(Tune*)), SLOT(actDownloadTune(Tune*)));
}

void Qomp::setupPlayer()
{
#ifdef HAVE_PHONON
	player_ = new QompPhononPlayer(this);
#elif HAVE_QTMULTIMEDIA
	player_ = new QompQtMultimediaPlayer(this);
#endif

}

void Qomp::setupModel()
{
	model_ = new QompPlayListModel(this);
	model_->restoreState();
}
