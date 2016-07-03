/*
 * Copyright (C) 2013-2016  Khryukin Evgeny
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
#include "updateschecker.h"
#include "qomptunedownloader.h"
#include "gettuneurlhelper.h"
#include "qompcommandline.h"
#include "qompinstancewatcher.h"
#ifndef Q_OS_ANDROID
#include "aboutdlg.h"
#include "thememanager.h"
#include <QApplication>
#else
#include "qompqmlengine.h"

//#define TEST_ANDROID

#include <qqml.h>
#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QtAndroid>
#include <QGuiApplication>
#ifdef TEST_ANDROID
#include <QFile>
#endif
#endif

#include <QThread>

#include <QTimer>
#include <QDesktopServices>
#include <QClipboard>

#ifdef HAVE_QTMULTIMEDIA
#include "qompqtmultimediaplayer.h"
#endif

#ifdef Q_OS_MAC
#include "CocoaTrayClick.h"
#include <QFileOpenEvent>
#endif

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const int ADDING_INTERVAL = 2000;

#ifdef Q_OS_ANDROID
static QompCon* _instance;

#ifdef TEST_ANDROID
static QFile f("/sdcard/.qomp/log.txt");
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QString str = QString("Debug: %1 (%2:%3, %4)\n")
				.arg(msg)
				.arg(context.file)
				.arg(context.line)
				.arg(context.function);
	f.write(str.toLocal8Bit());
	if (type == QtFatalMsg) {
		abort();
	}
}
#endif

static void incomingCallStart(JNIEnv */*env*/, jobject /*thiz*/)
{
	QMetaObject::invokeMethod(_instance, "incomingCall", Qt::QueuedConnection, Q_ARG(bool, true));
}

static void incomingCallFinish(JNIEnv */*env*/, jobject /*thiz*/)
{
	QMetaObject::invokeMethod(_instance, "incomingCall", Qt::QueuedConnection, Q_ARG(bool, false));
}

static void notifyIcon(const QString& text)
{
	QAndroidJniObject str = QAndroidJniObject::fromString(text);
	QAndroidJniObject act = QtAndroid::androidActivity();
	act.callObjectMethod("showStatusIcon", "(Ljava/lang/String;)V", str.object<jstring>());
}

static void deInitActivity()
{
	QAndroidJniObject::callStaticMethod<void>("net/sourceforge/qomp/Qomp",
						"deInit", "()V");

}

static void setUrl(JNIEnv */*env*/, jobject /*thiz*/, const jstring url)
{
	QAndroidJniObject obj(url);

#ifdef DEBUG_OUTPUT
	qDebug() << "setUrl " << obj.toString();
#endif
	QMetaObject::invokeMethod(_instance, "processUrl", Qt::QueuedConnection, Q_ARG(QString, obj.toString()));
}
#endif



QompCon::QompCon(QObject *parent) :
	QObject(parent),
	mainWin_(nullptr),
	model_(nullptr),
	player_(nullptr),
	commandLine_(nullptr)
{
	qRegisterMetaType<Tune*>("Tune*");
	qRegisterMetaType<Qomp::State>("State");
#ifdef Q_OS_ANDROID
	_instance = this;

#ifdef TEST_ANDROID
	if(f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate)) {
		qInstallMessageHandler(myMessageOutput);
	}
	else {
		qWarning() << "Error setup message handler!";
	}
#endif

	QAndroidJniObject act = QtAndroid::androidActivity();
	QAndroidJniEnvironment jni;
	jclass clazz = jni->GetObjectClass(act.object());
	JNINativeMethod methods[] = {
			{ "incomingCallStart",  "()V", (void*)incomingCallStart  },
			{ "incomingCallFinish", "()V", (void*)incomingCallFinish },
			{ "setUrl", "(Ljava/lang/String;)V", (void*)setUrl }
		};
	jni->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
	jni->DeleteLocalRef(clazz);

	qmlRegisterType<UpdatesChecker>("net.sourceforge.qomp", 1, 0, "UpdatesChecker");
#endif

	commandLine_ = new QompCommandLine(this);
	if(!setupWatcher())
		return;

	connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), SLOT(applicationStateChanged(Qt::ApplicationState)));
	connect(qApp, SIGNAL(aboutToQuit()), SLOT(deInit()));

#ifdef DEBUG_OUTPUT
	qDebug() << "QompCon creation";
#endif

#ifdef Q_OS_MAC
	qApp->installEventFilter(this);
#endif

	QTimer::singleShot(0, this, SLOT(init()));
}

QompCon::~QompCon()
{
#ifdef TEST_ANDROID
	f.flush();
	f.close();
#endif
}

void QompCon::applicationStateChanged(Qt::ApplicationState state)
{
	switch(state) {
	case Qt::ApplicationActive:
		break;
	case Qt::ApplicationInactive:
		model_->saveState();
		break;
	default:
		break;
	}
}

void QompCon::preparePlayback()
{
	auto pConn = QSharedPointer<QMetaObject::Connection>::create();
	*pConn = connect(player_, &QompPlayer::mediaReady,
			 [this, pConn]()
		{
	#ifdef DEBUG_OUTPUT
			qDebug() << "QompCon::init lambda";
	#endif
			Options* o = Options::instance();

			if(o->getOption(OPTION_REMEMBER_POS).toBool()) {
				const qint64 pos = o->getOption(OPTION_LAST_POS).toLongLong();
				player_->setPosition(pos);
				mainWin_->setCurrentPosition(pos);
			}

			if(o->getOption(OPTION_AUTOSTART_PLAYBACK).toBool()) {
				player_->play();
			}

			disconnect(*pConn);
		}
	);
}

void QompCon::processCommandLine()
{
	foreach(const QString& arg, commandLine_->args()) {
		QMetaObject::invokeMethod(this, "processUrl", Qt::QueuedConnection, Q_ARG(QString, arg));
	}

	commandLine_->deleteLater();
	commandLine_ = nullptr;
}

bool QompCon::setupWatcher()
{
#ifndef QOMP_MOBILE
	connect(&watcher_, &QompInstanceWatcher::commandTune, this, &QompCon::processUrl, Qt::QueuedConnection);

	if(!watcher_.newInstanceAllowed()) {
		foreach(const QString& arg, commandLine_->args())
			watcher_.sendCommandTune(arg);

		QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
		return false;
	}
#endif
	return true;
}

void QompCon::init()
{
	checkVersion();
	updateSettings();

	setupModel();

	setupPlayer();
	setupMainWin();

	model_->restoreState();

	Options* o = Options::instance();

	connect(o, SIGNAL(updateOptions()), SLOT(updateSettings()));

#ifndef Q_OS_ANDROID
	if(o->getOption(OPTION_START_MINIMIZED).toBool())
		mainWin_->hide();
	else
		mainWin_->show();
#endif

	preparePlayback();
	processCommandLine();
}

void QompCon::savePlayerPosition(qint64 pos)
{
	Options* o = Options::instance();

	if(!o->getOption(OPTION_REMEMBER_POS).toBool())
		pos = 0;

	o->setOption(OPTION_LAST_POS, pos);
}

void QompCon::savePlayerPosition()
{
	savePlayerPosition(player_->position());
}

void QompCon::deInit()
{
	qint64 pos = player_->position();

	stopPlayer();
	model_->saveState();

	savePlayerPosition(pos);

	Options::instance()->setOption(OPTION_VOLUME, player_->volume());

	delete mainWin_;
	delete player_;
	delete Tune::emptyTune();
	delete model_;

	//we do this cleanup here cos sometimes on android
	//we catch random crashes on exit;
	delete Options::instance();
	delete Translator::instance();
#ifndef Q_OS_ANDROID
	delete ThemeManager::instance();
#else
	deInitActivity();
	delete QompQmlEngine::instance();
#endif
}

void QompCon::processUrl(const QString &url)
{
	//When we open several songs, this slot will be
	//called several times.
	//We want to play the first song
	static bool adding = false;

	QList<Tune*> tunes;
	if(PluginManager::instance()->processUrl(url, &tunes)) {
		model_->addTunes(tunes);
		if(!adding && tunes.size() > 0) {
			stopPlayer();
			//start playback from the begining
			Options::instance()->setOption(OPTION_LAST_POS, 0);
			model_->setCurrentTune(tunes.at(0));
			player_->setPosition(0);
			player_->play();

			adding = true;

			QTimer::singleShot(ADDING_INTERVAL, [](){
				adding = false;
			});
		}
	}
}

bool QompCon::eventFilter(QObject *obj, QEvent *e)
{
#ifdef Q_OS_MAC
	if(obj == qApp && e->type() == QEvent::FileOpen) {
		QFileOpenEvent *fo = static_cast<QFileOpenEvent *>(e);
		const QString file = fo->file();
		if(!file.isEmpty()) {
			processUrl(file);
		}
	}
#endif
	return QObject::eventFilter(obj, e);
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
		hash.insert(OPTION_SHUFFLE,		false);
		hash.insert(OPTION_PLUGINS_ORDER,	QStringList()	<< "Myzuka.ru"
									<< "Yandex.Music"
									<< "Pleer.com");
		hash.insert(OPTION_THEME,
#ifdef Q_OS_MAC
					"default"
#else
					"blue"
#endif
						);
		hash.insert(OPTION_CURRENT_TRANSLATION, QLocale::system().name().split("_").first());
		hash.insert(OPTION_MUTED, false);
		hash.insert(OPTION_REPEAT_LAST_SEARCH, false);
		hash.insert(OPTION_REMEMBER_POS, true);
		hash.insert(OPTION_LAST_POS, 0);
		hash.insert(OPTION_PLAYLIST_VISIBLE, true);
		hash.insert(OPTION_PLAYLIST_HEIGHT, 0);
		hash.insert(OPTION_ONE_COPY, true);

		foreach(const char* key, hash.keys()) {
			if(Options::instance()->getOption(key) == QVariant::Invalid)
				Options::instance()->setOption(key, hash.value(key));
		}

		Options::instance()->setOption(OPTION_APPLICATION_VERSION, APPLICATION_VERSION);
	}
}

void QompCon::incomingCall(bool begining)
{
#ifdef Q_OS_ANDROID
	static Qomp::State state = Qomp::StateStopped;

	if(begining) {
		if(player_->state() == Qomp::StatePlaying) {
			state = Qomp::StatePlaying;
			actPause();
		}
	}
	else {
		if(state == Qomp::StatePlaying) {
			actPlay();
			state =  Qomp::StateStopped;
		}
	}
#else
	Q_UNUSED(begining)
#endif
}

void QompCon::updateSettings()
{
	QompNetworkingFactory::instance()->updateProxySettings();
#ifndef Q_OS_ANDROID
	ThemeManager::instance()->setTheme(Options::instance()->getOption(OPTION_THEME).toString());
#endif
	Translator::instance()->retranslate(Options::instance()->getOption(OPTION_CURRENT_TRANSLATION).toString());

	//if some plugins were enabled - set player for them
	PluginManager::instance()->qompPlayerChanged(player_);
}

void QompCon::actPlayNext()
{
	QModelIndex index = model_->currentIndex();
	if(index.isValid() && index.row() < model_->rowCount()-1) {
		Qomp::State state = player_->state();
		index = model_->index(index.row()+1);
		if(state != Qomp::StateStopped) {
			stopPlayer();
		}
		model_->setCurrentTune(model_->tune(index));
		if(state == Qomp::StatePlaying) {
			actPlay();
		}
	}
}

void QompCon::actPlayPrev()
{
	QModelIndex index = model_->currentIndex();
	if(index.isValid() && index.row() > 0) {
		Qomp::State state = player_->state();
		index = model_->index(index.row()-1);		
		if(state != Qomp::StateStopped) {
			stopPlayer();			
		}
		model_->setCurrentTune(model_->tune(index));
		if(state == Qomp::StatePlaying) {
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
		Options::instance()->setOption(OPTION_MUTED, mute);
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
	UpdatesChecker* uc = new UpdatesChecker(this);
	connect(uc, SIGNAL(finished()), uc, SLOT(deleteLater()));
	uc->startCheck();
#endif
}

void QompCon::actAboutQomp()
{
#ifndef Q_OS_ANDROID
	new AboutDlg(0);
#endif
}

void QompCon::actBugReport()
{
	static const QUrl bugUrl("https://sourceforge.net/p/qomp/tickets/new/");
	QDesktopServices::openUrl(bugUrl);
}

void QompCon::actDownloadTune(Tune *tune, const QString &dir)
{
	QompTuneDownloader *td = new QompTuneDownloader(this);
	td->download(tune, dir);
}

void QompCon::actToggleTuneState(Tune *tune)
{
	model_->setCurrentTune(tune);
	actPlay();
}

void QompCon::actRemoveTune(Tune *tune)
{
	if(model_->currentTune() == tune) {
		stopPlayer();
		model_->setCurrentTune(model_->tune(model_->index(0,0)));
	}
	model_->removeTune(tune);
}

void QompCon::actCopyUrl(Tune *tune)
{
	GetTuneUrlHelper* helper = new GetTuneUrlHelper(this, "tuneUrlFinished", this);
	helper->getTuneUrlAsynchronously(tune);
}

void QompCon::tuneUrlFinished(const QUrl &url)
{
	qApp->clipboard()->setText(url.toString());
}

void QompCon::setupMainWin()
{
	mainWin_ = new QompMainWin(qApp);
	mainWin_->setModel(model_);

#ifndef Q_OS_ANDROID
	mainWin_->setMuteState(player_->isMuted());
	mainWin_->volumeChanged(player_->volume());
#endif
	mainWin_->setCurrentPosition(player_->position());
	mainWin_->playerStateChanged(player_->state());

	connectMainWin();

	mainWin_->updateButtonIcons();
}

void QompCon::connectMainWin()
{
	connect(player_, SIGNAL(currentPositionChanged(qint64)),	mainWin_, SLOT(setCurrentPosition(qint64)));
	connect(player_, SIGNAL(mutedChanged(bool)),			mainWin_, SLOT(setMuteState(bool)));
	connect(player_, SIGNAL(volumeChanged(qreal)),			mainWin_, SLOT(volumeChanged(qreal)));
	connect(player_, SIGNAL(currentTuneTotalTimeChanged(qint64)),	mainWin_, SLOT(currentTotalTimeChanged(qint64)));

	connect(mainWin_, SIGNAL(aboutQomp()),				SLOT(actAboutQomp()));
	connect(mainWin_, SIGNAL(bugReport()),				SLOT(actBugReport()));
	connect(mainWin_, SIGNAL(checkForUpdates()),			SLOT(actCheckForUpdates()));
	connect(mainWin_, SIGNAL(doOptions()),				SLOT(actDoSettings()));
	connect(mainWin_, SIGNAL(downloadTune(Tune*,QString)),		SLOT(actDownloadTune(Tune*,QString)));
	connect(mainWin_, SIGNAL(copyUrl(Tune*)),			SLOT(actCopyUrl(Tune*)));
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
#ifndef QOMP_MOBILE
	connect(&watcher_, SIGNAL(commandShow()), mainWin_, SLOT(show()));
#endif

#ifdef Q_OS_MAC
	connect(CocoaTrayClick::instance(), SIGNAL(trayClicked()), mainWin_, SLOT(toggleVisibility()));
#endif

#ifndef QOMP_MOBILE
	connect(ThemeManager::instance(), SIGNAL(themeChanged()), mainWin_, SLOT(updateButtonIcons()));
#endif
}

void QompCon::disconnectMainWin()
{
	mainWin_->disconnect(this);
	player_->disconnect(mainWin_);
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
	player_->setMute(Options::instance()->getOption(OPTION_MUTED).toBool());
#endif
}

void QompCon::setupModel()
{
	model_ = new QompPlayListModel(this);
	connect(model_, SIGNAL(currentTuneChanged(Tune*)), SLOT(currentTuneChanged(Tune*)));
}

void QompCon::stopPlayer()
{
	player_->blockSignals(true);
	player_->stop();
	mainWin_->setCurrentPosition(0);
	playerStateChanged(Qomp::StateStopped);

	while (player_->state() != Qomp::StateStopped) {
		QThread::msleep(1);
		qApp->processEvents();
	}

	savePlayerPosition();
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
#ifdef HAVE_QTMULTIMEDIA
	return new QompQtMultimediaPlayer;
#endif
	return nullptr;
}

void QompCon::playNextShuffle(bool afterError, const QModelIndex &index)
{
	QModelIndex ind;

	bool finished = model_->allTunesPlayed();

	if(finished) {
		model_->unsetAllTunesPlayed();
	}

	if( !finished || (!afterError && Options::instance()->getOption(OPTION_REPEAT_ALL).toBool()) ) {
		if(player_->lastAction() == Qomp::StatePlaying) {
			int r = index.row();

			if(model_->rowCount() > 1) {
				while(index.row() == r || model_->tune(model_->index(r))->played) {
					r = rand() % model_->rowCount();
				}
			}

			ind = model_->index(r);
		}
	}

	if(ind.isValid())
		playIndex(ind);
	else
		stopPlayer();
}

void QompCon::playNext(bool afterError, const QModelIndex &index)
{
	if(index.row() == model_->rowCount()-1) {
		if(!afterError && player_->lastAction() == Qomp::StatePlaying &&
				Options::instance()->getOption(OPTION_REPEAT_ALL).toBool())
		{
			const QModelIndex ind = model_->index(0);
			playIndex(ind);

		}
		else {
			stopPlayer();
			model_->setCurrentTune(const_cast<Tune*>(Tune::emptyTune()));
		}
	}
	else {
		if(player_->lastAction() == Qomp::StatePlaying) {
			const QModelIndex ind = model_->index(index.row()+1);
			playIndex(ind);
		}
		else {
			stopPlayer();
		}
	}
}

void QompCon::mediaFinished(bool afterError)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompCon::mediaFinished()";
#endif
	mainWin_->setCurrentPosition(0);
	savePlayerPosition();

	QModelIndex index = model_->currentIndex();

	//If current tune is empty tune, we probably reachs the end of playlist
	if(!index.isValid())
		return;

	if(Options::instance()->getOption(OPTION_SHUFFLE).toBool()) {
		model_->currentTune()->played = true;
		playNextShuffle(afterError, index);
	}
	else {
		playNext(afterError, index);
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

void QompCon::currentTuneChanged(Tune *t)
{
	Q_UNUSED(t);
#ifdef Q_OS_ANDROID
	notifyIcon(model_->indexForTune(t).data().toString());
#endif
}
