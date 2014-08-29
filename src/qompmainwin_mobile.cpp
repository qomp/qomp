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
	void indexActivate(const QJSValue& val);
	void doToggle(const QJSValue& val);
	void removeTune(const QJSValue& val);
	void downloadTune(const QJSValue& index, const QJSValue &dir);
	void loadPlaylistActivate(const QJSValue& file);
	void savePlaylistActivate(const QJSValue& file);

	void updateTuneInfo(Tune *tune);
	void updateState(Qomp::State state);

	void updateOptions(const QJSValue &val);
	void totalDurationChanged(uint time);


private slots:
	void sliderMoved(const QJSValue &val);
	void actionTriggered();
	void buildOpenTunesMenu();

private:
	QModelIndex jsValueToIndex(const QJSValue &val);

public:
	QompMainWin* mainWin_;
	QQuickItem* root_;
};


QompMainWin::Private::Private(QompMainWin *p) :
	QObject(p),
	mainWin_(p)
{
}

QompMainWin::Private::~Private()
{
	delete root_;
}

void QompMainWin::Private::setUp()
{
	root_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/MainWin.qml"));
	QQmlProperty::write(root(), "playlistModel", QVariant::fromValue(mainWin_->model_ ));
	QompQmlEngine::instance()->addItem(root());

	buildOpenTunesMenu();

	QQmlProperty::write(root(), "repeat", Options::instance()->getOption(OPTION_REPEAT_ALL));
	QQmlProperty::write(root(), "currentFolder", QUrl::fromLocalFile(Options::instance()->getOption(LAST_DIR).toString()));

	connect(root(), SIGNAL(positionChanged(QJSValue)), SLOT(sliderMoved(QJSValue)));
	connectActions();
	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(buildOpenTunesMenu()));

	totalDurationChanged(0);
}

void QompMainWin::Private::connectActions()
{
	QQuickItem* r = root();
	connect(r, SIGNAL(actRepeat(QJSValue)),		SLOT(updateOptions(QJSValue)));
	connect(r, SIGNAL(actDoOpenMenu()),		SLOT(actOpenActivated()));
	connect(r, SIGNAL(itemActivated(QJSValue)),	SLOT(indexActivate(QJSValue)));
	connect(r, SIGNAL(actToggle(QJSValue)),		SLOT(doToggle(QJSValue)));
	connect(r, SIGNAL(actDownload(QJSValue,QJSValue)),	SLOT(downloadTune(QJSValue,QJSValue)));
	connect(r, SIGNAL(actRemove(QJSValue)),		SLOT(removeTune(QJSValue)));

	connect(r, SIGNAL(actLoadPlaylist(QJSValue)),	SLOT(loadPlaylistActivate(QJSValue)));
	connect(r, SIGNAL(actSavePlaylist(QJSValue)),	SLOT(savePlaylistActivate(QJSValue)));

	connect(r, SIGNAL(actClearPlaylist()),		SLOT(actClearActivated()));

	connect(r, SIGNAL(actPlay()), mainWin_,  SIGNAL(actPlayActivated()));
	connect(r, SIGNAL(actStop()), mainWin_,  SIGNAL(actStopActivated()));	
	connect(r, SIGNAL(actNext()), mainWin_,  SIGNAL(actNextActivated()));
	connect(r, SIGNAL(actPrev()), mainWin_,  SIGNAL(actPrevActivated()));
	connect(r, SIGNAL(actExit()), mainWin_,  SIGNAL(exit()));

	//connect(ui->tbOptions, SIGNAL(clicked()), mainWin_, SIGNAL(doOptions()));
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

void QompMainWin::Private::sliderMoved(const QJSValue &val)
{
	mainWin_->seekSliderMoved(val.toUInt());
}

void QompMainWin::Private::actionTriggered()
{
	QompPluginAction* act = static_cast<QompPluginAction*>(sender());
	QList<Tune*> t = act->getTunes();	
	if(!t.isEmpty())
		emit mainWin_->tunes(t);
}

void QompMainWin::Private::buildOpenTunesMenu()
{
	QObjectList l = QQmlProperty::read(root(), "pluginsActions").value<QObjectList>();
	while(!l.isEmpty())
		l.takeFirst()->deleteLater();

	foreach(QompPluginAction* act, PluginManager::instance()->tunesActions()) {
		act->setParent(this);
		connect(act, SIGNAL(triggered()), SLOT(actionTriggered()));
		l << act;
	}
	QQmlProperty::write(root(), "pluginsActions", QVariant::fromValue(l));
}

QModelIndex QompMainWin::Private::jsValueToIndex(const QJSValue &val)
{
	return mainWin_->model_->index(val.toInt());
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

void QompMainWin::Private::indexActivate(const QJSValue &val)
{
	emit mainWin_->mediaActivated(jsValueToIndex(val));
}

void QompMainWin::Private::doToggle(const QJSValue& val)
{
	emit mainWin_->toggleTuneState(mainWin_->model_->tune(jsValueToIndex(val)));
}

void QompMainWin::Private::removeTune(const QJSValue &val)
{
	emit mainWin_->removeTune(mainWin_->model_->tune(jsValueToIndex(val)));
}

void QompMainWin::Private::downloadTune(const QJSValue &row, const QJSValue &dir)
{
	emit mainWin_->downloadTune(mainWin_->model_->tune(jsValueToIndex(row)), dir.toString());
}

void QompMainWin::Private::loadPlaylistActivate(const QJSValue &file)
{
	mainWin_->loadPlaylist(file.toVariant().toUrl().toLocalFile());
}

void QompMainWin::Private::savePlaylistActivate(const QJSValue &file)
{
	mainWin_->savePlaylist(file.toVariant().toUrl().toLocalFile());
}

void QompMainWin::Private::updateTuneInfo(Tune* tune)
{
	QQmlProperty::write(root(), "currentDurationText", tune->duration);
	QMetaObject::invokeMethod(root(), "enshureItemVisible",
				  Q_ARG(QVariant, mainWin_->model_->indexForTune(tune).row()));
}

void QompMainWin::Private::updateState(Qomp::State state)
{
	switch (state) {
	case Qomp::StatePlaying:
		QQmlProperty::write(root(), "playing", true);
		break;

	case Qomp::StateLoading:
	//case Qomp::StateBuffering:
		QQmlProperty::write(root(), "busy", true);
		break;

	case Qomp::StatePaused:
	case Qomp::StateStopped:
		QQmlProperty::write(root(), "playing", false);
		QQmlProperty::write(root(), "busy", false);
		break;
//	case Qomp::StateError:
	default:
		QQmlProperty::write(root(), "busy", false);
		break;
	}
}

void QompMainWin::Private::updateOptions(const QJSValue& val)
{
	Options::instance()->setOption(OPTION_REPEAT_ALL, val.toBool());
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

	if(ms == -1 || ms == 0)
		return;
	model_->currentTune()->duration = Qomp::durationMiliSecondsToString(ms);
	model_->tuneDataUpdated(model_->currentTune());
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

#include "qompmainwin_mobile.moc"
