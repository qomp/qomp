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

#include "qompmainwin.h"
#include "qompplaylistmodel.h"
#include "options.h"
#include "defines.h"
#include "tune.h"
#include "qompqmlengine.h"
#include "qomppluginaction.h"
#include "pluginmanager.h"


#include <QCloseEvent>
#include <QTime>
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlComponent>
#include <QFileInfo>
#include <QQmlProperty>




class QompMainWin::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QompMainWin* p);
	~Private();

	void setUp();
	QQuickItem* root() const;

	void connectActions();
	void setCurrentPosition(quint64 pos);
	void setCurrentDuration(quint64 dur);

public slots:
	void actOpenActivated();
	void actClearActivated();
	void indexActivate(int val);
	void doToggle(int val);
	void removeTune(int val);
	void downloadTune(int index, const QString &dir);
	void loadPlaylistActivate(const QString& file);
	void savePlaylistActivate(const QString& file);
	void copyUrl(const QVariant& tune);

	void updateTuneInfo(Tune *tune);
	void updateState(Qomp::State state);

	void updateRepeatState(bool val);
	void updateShuffleState(bool val);
	void totalDurationChanged(uint time);


private slots:
	void sliderMoved(int val);
	void buildOpenTunesMenu();

private:
	QModelIndex intToIndex(int val);

public:
	QompMainWin* mainWin_;
	QQuickItem* root_;
};


QompMainWin::Private::Private(QompMainWin *p) :
	QObject(p),
	mainWin_(p),
	root_(nullptr)
{
}

QompMainWin::Private::~Private()
{
	//will be deleted by js engine
	//delete root_;
}

void QompMainWin::Private::setUp()
{
	root_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/MainWin.qml"));
	QQmlProperty::write(root(), "playlistModel", QVariant::fromValue(mainWin_->model_ ));
	QompQmlEngine::instance()->addItem(root());

	buildOpenTunesMenu();

	QQmlProperty::write(root(), "repeat", Options::instance()->getOption(OPTION_REPEAT_ALL));
	QQmlProperty::write(root(), "shuffle", Options::instance()->getOption(OPTION_SHUFFLE));
	QQmlProperty::write(root(), "currentFolder", QUrl::fromLocalFile(Qomp::safeDir(Options::instance()->getOption(LAST_DIR).toString())));

	connect(root(), SIGNAL(positionChanged(int)), SLOT(sliderMoved(int)));
	connectActions();
	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(buildOpenTunesMenu()));

	totalDurationChanged(0);
}

void QompMainWin::Private::connectActions()
{
	QQuickItem* r = root();
	connect(r, SIGNAL(actRepeat(bool)),		SLOT(updateRepeatState(bool)));
	connect(r, SIGNAL(actShuffle(bool)),		SLOT(updateShuffleState(bool)));
	connect(r, SIGNAL(actDoOpenMenu()),		SLOT(actOpenActivated()));
	connect(r, SIGNAL(itemActivated(int)),		SLOT(indexActivate(int)));
	connect(r, SIGNAL(actToggle(int)),		SLOT(doToggle(int)));
	connect(r, SIGNAL(actDownload(int,QString)),	SLOT(downloadTune(int,QString)));
	connect(r, SIGNAL(actRemove(int)),		SLOT(removeTune(int)));
	connect(r, SIGNAL(actCopyUrl(QVariant)),	SLOT(copyUrl(QVariant)));

	connect(r, SIGNAL(actLoadPlaylist(QString)),	SLOT(loadPlaylistActivate(QString)));
	connect(r, SIGNAL(actSavePlaylist(QString)),	SLOT(savePlaylistActivate(QString)));

	connect(r, SIGNAL(actClearPlaylist()),		SLOT(actClearActivated()));

	connect(r, SIGNAL(actPlay()),		mainWin_, SIGNAL(actPlayActivated()));
	connect(r, SIGNAL(actStop()),		mainWin_, SIGNAL(actStopActivated()));
	connect(r, SIGNAL(actNext()),		mainWin_, SIGNAL(actNextActivated()));
	connect(r, SIGNAL(actPrev()),		mainWin_, SIGNAL(actPrevActivated()));
	connect(r, SIGNAL(actDoOptions()),	mainWin_, SIGNAL(doOptions()));
}

void QompMainWin::Private::setCurrentPosition(quint64 pos)
{
	QQmlProperty::write(root(), "currentPosition", pos);
	QQmlProperty::write(root(), "currentPositionText", Qomp::durationMiliSecondsToString(pos));
}

void QompMainWin::Private::setCurrentDuration(quint64 dur)
{
	QQmlProperty::write(root(), "currentDuration", dur);
	QQmlProperty::write(root(), "currentDurationText", Qomp::durationMiliSecondsToString(dur));
}

QQuickItem *QompMainWin::Private::root() const
{
	return root_;
}

void QompMainWin::Private::sliderMoved(int val)
{
	mainWin_->seekSliderMoved(val);
}

void QompMainWin::Private::buildOpenTunesMenu()
{
	QObjectList l = QQmlProperty::read(root(), "pluginsActions").value<QObjectList>();
	while(!l.isEmpty())
		l.takeFirst()->deleteLater();

	foreach(QompPluginAction* act, PluginManager::instance()->tunesActions()) {
		act->setParent(this);
		connect(act, &QompPluginAction::tunesReady, mainWin_, &QompMainWin::tunes);
		l << act;
	}
	QQmlProperty::write(root(), "pluginsActions", QVariant::fromValue(l));
}

QModelIndex QompMainWin::Private::intToIndex(int val)
{
	return mainWin_->model_->index(val);
}

void QompMainWin::Private::actOpenActivated()
{
	QMetaObject::invokeMethod(root(), "doOpenTunesMenu");
}

void QompMainWin::Private::actClearActivated()
{
	emit mainWin_->clearPlaylist();

	updateTuneInfo(mainWin_->model_->currentTune());
}

void QompMainWin::Private::indexActivate(int val)
{
	emit mainWin_->mediaActivated(intToIndex(val));
}

void QompMainWin::Private::doToggle(int val)
{
	emit mainWin_->toggleTuneState(mainWin_->model_->tune(intToIndex(val)));
}

void QompMainWin::Private::removeTune(int val)
{
	emit mainWin_->removeTune(mainWin_->model_->tune(intToIndex(val)));
}

void QompMainWin::Private::downloadTune(int row, const QString &dir)
{
	emit mainWin_->downloadTune(mainWin_->model_->tune(intToIndex(row)), QUrl(dir).toLocalFile());
}

void QompMainWin::Private::loadPlaylistActivate(const QString &file)
{
	mainWin_->loadPlaylist(file);
}

void QompMainWin::Private::savePlaylistActivate(const QString &file)
{
	mainWin_->savePlaylist(file);
}

void QompMainWin::Private::copyUrl(const QVariant &tune)
{
	Tune* t = static_cast<Tune*>(tune.value<QObject*>());
	emit mainWin_->copyUrl(t);
}

void QompMainWin::Private::updateTuneInfo(Tune* tune)
{
	QQmlProperty::write(root(), "currentDurationText", tune->duration);
	QMetaObject::invokeMethod(root(), "enshureItemVisible",
				  Q_ARG(QVariant, mainWin_->model_->indexForTune(tune).row()));
}

void QompMainWin::Private::updateState(Qomp::State state)
{
	bool play = false;
	bool busy = false;

	switch (state) {
	case Qomp::StatePlaying:
	case Qomp::StateBuffering:
		play = true;
		break;
	case Qomp::StateLoading:
		busy = true;
		break;
	case Qomp::StatePaused:
	case Qomp::StateStopped:
	default:
		break;
	}

	QQmlProperty::write(root(), "playing", play);
	QQmlProperty::write(root(), "busy", busy);
}

void QompMainWin::Private::updateRepeatState(bool val)
{
	Options::instance()->setOption(OPTION_REPEAT_ALL, val);
}

void QompMainWin::Private::updateShuffleState(bool val)
{
	Options::instance()->setOption(OPTION_SHUFFLE, val);
}

void QompMainWin::Private::totalDurationChanged(uint time)
{
	QTime t = QTime(0,0,0,0).addSecs(time);
	QQmlProperty::write(root(), "totalDuration", t.toString("hh:mm:ss"));
}




QompMainWin::QompMainWin(QObject *parent) :
	QObject(parent),
	d(new Private(this)),
	model_(0),
	currentState_(Qomp::StateUnknown)
{
}

QompMainWin::~QompMainWin()
{
	delete d;
	d = 0;
}

void QompMainWin::setModel(QompPlayListModel *model)
{
	model_ = model;
	d->setUp();
	connect(model_, SIGNAL(currentTuneChanged(Tune*)), d, SLOT(updateTuneInfo(Tune*)));
	connect(model_, SIGNAL(totalTimeChanged(uint)), d, SLOT(totalDurationChanged(uint)));
}

void QompMainWin::hide()
{
}

void QompMainWin::show()
{
}

void QompMainWin::setMuteState(bool/* mute*/)
{
}

void QompMainWin::volumeChanged(qreal/* vol*/)
{
}

void QompMainWin::playerStateChanged(Qomp::State state)
{
	if(currentState_ == state)
		return;

	currentState_ = state;
	d->updateState(state);
	d->updateTuneInfo(model_->currentTune());
}

void QompMainWin::setCurrentPosition(qint64 ms)
{
	d->setCurrentPosition(ms);
}

void QompMainWin::currentTotalTimeChanged(qint64 ms)
{
	d->setCurrentDuration(ms);
}

void QompMainWin::toggleVisibility()
{
}

bool QompMainWin::eventFilter(QObject *o, QEvent *e)
{
	return QObject::eventFilter(o,e);
}

void QompMainWin::loadPlaylist(const QString &fileName)
{
	Options::instance()->setOption(LAST_DIR, QFileInfo(fileName).absolutePath());
	model_->loadTunes(fileName);
}

void QompMainWin::savePlaylist(const QString &fileName)
{
	Options::instance()->setOption(LAST_DIR, QFileInfo(fileName).absolutePath());
	model_->saveTunes(fileName);
}

void QompMainWin::updateButtonIcons()
{
}

#include "qompmainwin_mobile.moc"
