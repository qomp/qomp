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

#include "qompmainwin.h"
#include "playlistmodel.h"
#include "options.h"
#include "pluginmanager.h"
#include "qomptrayicon.h"
#include "options/qompoptionsdlg.h"
#include "defines.h"
#include "common.h"
#include "qompmetadataresolver.h"
#include "aboutdlg.h"
#include "qomptunedownloader.h"
#include "qompplaylistdelegate.h"

#include "ui_qompmainwin.h"

#include <QTime>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QClipboard>
#include <QMenu>

static const QString cachedPlayListFileName = "/qomp-cached-playlist.qomp";


QompMainWin::QompMainWin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QompMainWin),
	player_(new QompPlayer(this)),
	model_(new PlayListModel(this)),
	trayIcon_(new QompTrayIcon(this)),
	resolver_(new QompMetaDataResolver(this)),
	currentState_(QompPlayer::StateStopped)
{
	ui->setupUi(this);

	player_->setSeekSlider(ui->seekSlider);
	player_->setVolumeSlider(ui->volumeSlider);

	ui->playList->setModel(model_);
	ui->playList->setItemDelegate(new QompPlaylistDelegate(this));

	TuneList tl = Tune::tunesFromFile(QDesktopServices::storageLocation(QDesktopServices::CacheLocation) + cachedPlayListFileName);
	if(!tl.isEmpty()) {
		model_->addTunes(tl);
		QModelIndex ind = model_->index(Options::instance()->getOption(OPTION_CURRENT_TRACK, 0).toInt(),0);
		model_->setCurrentTune(model_->tune(ind));
		ui->playList->setCurrentIndex(ind);
	}

	ui->tb_next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->tb_prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->tb_stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->tb_clear->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
	ui->tb_load->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->tb_save->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	ui->tb_open->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));

	setCurrentPosition(0);

	connect(ui->tb_open, SIGNAL(clicked()), SLOT(actOpenActivated()));
	connect(ui->tb_play, SIGNAL(clicked()), SLOT(actPlayActivated()));
	connect(ui->tb_stop, SIGNAL(clicked()), SLOT(actStopActivated()));
	connect(ui->tb_clear, SIGNAL(clicked()), SLOT(actClearActivated()));
	connect(ui->tb_next, SIGNAL(clicked()), SLOT(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()), SLOT(actPrevActivated()));
	connect(ui->tb_load, SIGNAL(clicked()), SLOT(loadPlaylist()));
	connect(ui->tb_save, SIGNAL(clicked()), SLOT(savePlaylist()));

	connect(ui->actionExit, SIGNAL(triggered()), SIGNAL(exit()));

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doMainContextMenu()));

	connect(ui->playList, SIGNAL(activated(QModelIndex)), SLOT(mediaActivated(QModelIndex)));
	connect(ui->playList, SIGNAL(clicked(QModelIndex)), SLOT(mediaClicked(QModelIndex)));
	connect(ui->playList, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doTrackContextMenu(QPoint)));

	connect(player_, SIGNAL(stateChanged(QompPlayer::State)), SLOT(playerStateChanged(QompPlayer::State)));
	connect(player_, SIGNAL(mediaFinished()), SLOT(playNext()));
	connect(player_, SIGNAL(currentPosition(qint64)), SLOT(setCurrentPosition(qint64)));

	connect(model_, SIGNAL(layoutChanged()), SLOT(updateTuneInfo()));

	connect(trayIcon_, SIGNAL(trayDoubleClicked()), SLOT(trayDoubleclicked()));
	connect(trayIcon_, SIGNAL(trayClicked(Qt::MouseButton)), SLOT(trayActivated(Qt::MouseButton)));

	connect(resolver_, SIGNAL(newMetaData(Tune,QMap<QString,QString>)), model_, SLOT(newDataReady(Tune,QMap<QString,QString>)));
	connect(resolver_, SIGNAL(newDuration(Tune,qint64)), model_, SLOT(totalTimeChanged(Tune,qint64)));

	resize(Options::instance()->getOption(OPTION_GEOMETRY_WIDTH, width()).toInt(),
		Options::instance()->getOption(OPTION_GEOMETRY_HEIGHT, height()).toInt());

	move(Options::instance()->getOption(OPTION_GEOMETRY_X, 10).toInt(),
		Options::instance()->getOption(OPTION_GEOMETRY_Y, 50).toInt());

	show();

	if(Options::instance()->getOption(OPTION_AUTOSTART_PLAYBACK).toBool())
		actPlayActivated();
}

QompMainWin::~QompMainWin()
{
	QDir dir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
	if(!dir.exists())
		dir.mkpath(dir.path());

	savePlaylist(QDesktopServices::storageLocation(QDesktopServices::CacheLocation) + cachedPlayListFileName);

	Options::instance()->setOption(OPTION_GEOMETRY_X, x());
	Options::instance()->setOption(OPTION_GEOMETRY_Y, y());
	Options::instance()->setOption(OPTION_GEOMETRY_HEIGHT, height());
	Options::instance()->setOption(OPTION_GEOMETRY_WIDTH, width());

	int curTrack = 0;
	QModelIndex ind = model_->indexForTune(model_->currentTune());
	if(ind.isValid())
		curTrack = ind.row();
	Options::instance()->setOption(OPTION_CURRENT_TRACK, curTrack);

	delete ui;
}

QompPlayer *QompMainWin::player() const
{
	return player_;
}

void QompMainWin::actPlayActivated()
{
	setCurrentPosition(0);

	if(!model_->rowCount())
		return;

	QModelIndex i = model_->indexForTune(model_->currentTune());
	if(!i.isValid()) {
		i = model_->index(0);
		ui->playList->setCurrentIndex(i);
		model_->setCurrentTune(model_->tune(i));
	}

	if(!(player_->currentTune() == model_->tune(i)))
		player_->setTune(model_->tune(i));

	player_->playOrPause();
	updateTuneInfo();
	if(currentState_ == QompPlayer::StatePlaing)
		currentState_ = QompPlayer::StatePaused;
	else
		currentState_ = QompPlayer::StatePlaing;
}

void QompMainWin::updateIcons()
{
	if(player_->state() == QompPlayer::StatePaused) {
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		trayIcon_->setIcon(QIcon(":/icons/icons/qomp_pause.png"));
	}
	else if(player_->state() == QompPlayer::StatePlaing) {
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		trayIcon_->setIcon(QIcon(":/icons/icons/qomp_play.png"));
	}
	else {
		trayIcon_->setIcon(QIcon(":/icons/icons/qomp_stop.png"));
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	}
}

void QompMainWin::actPrevActivated()
{
	QModelIndex index = model_->indexForTune(model_->currentTune());
	if(index.isValid() && index.row() > 0) {
		bool play = (player_->state() == QompPlayer::StatePlaing);
		index = model_->index(index.row()-1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		if(play)
			actPlayActivated();
	}
}

void QompMainWin::actNextActivated()
{
	QModelIndex index = model_->indexForTune(model_->currentTune());
	if(index.isValid() && index.row() < model_->rowCount()-1) {
		bool play = (player_->state() == QompPlayer::StatePlaing);
		index = model_->index(index.row()+1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		if(play)
			actPlayActivated();

	}
}

void QompMainWin::actStopActivated()
{
	player_->stop();
	setCurrentPosition(0);
	ui->lb_busy->stop();
	currentState_ = QompPlayer::StateStopped;
	QModelIndex index = ui->playList->currentIndex();
	if(index.isValid())
		model_->setCurrentTune(model_->tune(index));
	ui->lb_artist->setText("");
	ui->lb_title->setText("");
}

void QompMainWin::actOpenActivated()
{
	QMenu* m = buildPluginListMenu();
	m->move(QCursor::pos());
	QAction* x = m->exec();
	if(x)
		getTunes(x->text());

	m->deleteLater();
}

void QompMainWin::actClearActivated()
{
	QMenu m;
	QList<QAction*> acts;
	acts << new QAction(tr("Remove all"), &m)
	     << new QAction(tr("Remove Selected"), &m);
	m.addActions(acts);
	m.move(QCursor::pos());
	int x = acts.indexOf(m.exec());
	if(x == 0) {
		player_->stop();
		model_->clear();
	}
	else if(x == 1) {
		bool removingCurrent = false;
		QList<Tune> list;
		foreach(const QModelIndex& index, ui->playList->selectionModel()->selectedIndexes()) {
			Tune t = model_->tune(index);
			if(t == model_->currentTune())
				removingCurrent = true;
			list << t;
		}
		foreach(const Tune& tune, list)
			model_->removeTune(tune);

		if(removingCurrent) {
			player_->stop();
			model_->setCurrentTune(model_->tune(model_->index(0,0)));
		}
		ui->playList->setCurrentIndex(model_->indexForTune(model_->currentTune()));

	}
	updateTuneInfo();
}

void QompMainWin::mediaActivated(const QModelIndex &index)
{
	model_->setCurrentTune(model_->tune(index));
	actPlayActivated();
}

void QompMainWin::mediaClicked(const QModelIndex &index)
{
	if(player_->state() == QompPlayer::StateStopped
		|| player_->state() == QompPlayer::StateError)
	{
		model_->setCurrentTune(model_->tune(index));
	}
}

void QompMainWin::doTrackContextMenu(const QPoint &p)
{
	QModelIndex index = ui->playList->indexAt(p);
	if(!index.isValid())
		return;

	ui->playList->setCurrentIndex(index);
	Tune tune = model_->tune(index);
	QMenu menu;
	QList<QAction*> acts;
	acts << new QAction(tr("Play/Pause"), &menu);
	acts << new QAction(tr("Remove"), &menu);
	if(!tune.url.isEmpty()) {
		acts << new QAction(tr("Copy URL"), &menu);
		acts << new QAction(tr("Save file"), &menu);
	}
	menu.addActions(acts);
	menu.move(QCursor::pos());
	int x = acts.indexOf(menu.exec());
	if(x == 0) {
		model_->setCurrentTune(tune);
		actPlayActivated();
	}
	else if(x == 1) {
		model_->removeTune(tune);
		if(model_->currentTune() == tune) {
			model_->setCurrentTune(model_->tune(model_->index(0,0)));
		}
		ui->playList->setCurrentIndex(model_->indexForTune(model_->currentTune()));
	}
	else if(x == 2) {
		qApp->clipboard()->setText(tune.url);
	}
	else if(x == 3) {
		static const QString option = "main.last-save-dir";
		QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory"),
					Options::instance()->getOption(option, QDir::homePath()).toString());
		if(dir.isEmpty())
			return;
		Options::instance()->setOption(option, dir);
		QompTuneDownloader *td = new QompTuneDownloader(this);
		td->download(tune, dir);
	}

}

void QompMainWin::doMainContextMenu()
{
	QMenu *open = buildPluginListMenu();
	QMenu m;
	QList<QAction*> acts;
	if(isHidden()) {
		acts << new QAction(tr("Show"), &m);
	}
	else {
		acts << new QAction(tr("Hide"), &m);
	}
	acts << open->menuAction();
	acts << new QAction(tr("Settings"), &m);
	QAction* sep = new QAction(&m);
	sep->setSeparator(true);
	acts << sep;
	acts << new QAction(tr("Exit"), &m);
	m.addActions(acts);

	QMenu helpMenu(tr("Help"));
	QAction* about = new QAction(tr("About qomp"), &helpMenu);
	helpMenu.addAction(about);
	QAction* aboutQt = new QAction(tr("About Qt"), &helpMenu);
	helpMenu.addAction(aboutQt);
	m.insertMenu(acts.last(), &helpMenu);
	acts << about << aboutQt;

	m.move(QCursor::pos());
	QAction* x = m.exec();
	int ret = acts.indexOf(x);
	if(ret == 0)
		trayDoubleclicked();
	else if(ret == 2)
		doOptions();
	else if(ret == 4)
		emit exit();
	else if(x && x->parent() == open) {
		getTunes(x->text());
	}
	else if(ret == 5) {
		new AboutDlg(this);
	}
	else if(ret == 6) {
		qApp->aboutQt();
	}

	open->deleteLater();
}

void QompMainWin::playerStateChanged(QompPlayer::State state)
{
	updateIcons();
	ui->lb_busy->stop();
	if(currentState_ == QompPlayer::StateStopped)
		return;

	if(state == QompPlayer::StateError)
		playNext();
	else if(state == QompPlayer::StateBuffering) {
		ui->lb_busy->changeText(tr("Buffering"));
		ui->lb_busy->start();
	}
	else if(state == QompPlayer::StateLoading) {
		ui->lb_busy->changeText(tr("Loading"));
		ui->lb_busy->start();
	}
}

void QompMainWin::setCurrentPosition(qint64 ms)
{
	ui->lcd->display(durationMiliSecondsToString(ms));
}

void QompMainWin::playNext()
{
	setCurrentPosition(0);
	if(currentState_ == QompPlayer::StateStopped)
		return;

	if(model_->indexForTune(model_->currentTune()).row() == model_->rowCount()-1) {
		actStopActivated();
		model_->setCurrentTune(Tune());
	}
	else {
		QModelIndex index = model_->indexForTune(model_->currentTune());
		index = model_->index(index.row()+1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		actPlayActivated();
	}
}

void QompMainWin::doOptions()
{
	QompOptionsDlg dlg;
	dlg.exec();
}

void QompMainWin::loadPlaylist()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Select Playlist"),
						    Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), "*.qomp");
	if(!file.isEmpty()) {
		Options::instance()->setOption(LAST_DIR, file);
		TuneList tl = Tune::tunesFromFile(file);
		model_->addTunes(tl);
	}
}

void QompMainWin::savePlaylist()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save Playlist"),
						    Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), "*.qomp");
	if(!file.isEmpty()) {
		Options::instance()->setOption(LAST_DIR, file);
		savePlaylist(file);
	}
}

void QompMainWin::trayDoubleclicked()
{
	bool b = isHidden();
	if(b)
		show();
	else
		hide();
}

void QompMainWin::trayActivated(Qt::MouseButton b)
{
	if(b == Qt::RightButton) {
		doMainContextMenu();
	}
	else if(b == Qt::MidButton) {
		if(player_->state() == QompPlayer::StatePlaing || player_->state() == QompPlayer::StatePaused)
			player_->playOrPause();
	}
}

void QompMainWin::closeEvent(QCloseEvent *e)
{
	hide();
	e->ignore();
}

void QompMainWin::savePlaylist(const QString &fileName)
{
	QString f(fileName);
	if(!f.endsWith(".qomp"))
		f += ".qomp";
	QFile file(f);
	if(file.open(QFile::ReadWrite | QFile::Truncate)) {
		if(model_->rowCount() > 0) {
			QTextStream ts(&file);
			ts.setCodec("UTF-8");
			for(int i = 0; i < model_->rowCount(); i++) {
				QString str = model_->tune(model_->index(i)).toString();
				ts << str << endl;
			}
		}
	}
}

QMenu* QompMainWin::buildPluginListMenu()
{
	QMenu* m = new QMenu(tr("Open"), this);
	QList<QAction*> acts;
	foreach(const QString& name, PluginManager::instance()->availablePlugins()) {
		acts.append(new QAction(name, m));
	}
	m->addActions(acts);
	return m;
}

void QompMainWin::getTunes(const QString &name)
{
	TuneList list = PluginManager::instance()->getTune(name);
	if(!list.isEmpty()) {
		model_->addTunes(list);
		resolver_->resolve(list);
		if(player_->state() == QompPlayer::StateStopped
			|| player_->state() == QompPlayer::StateError)
		{
			QModelIndex index = model_->indexForTune(list.first());
			ui->playList->setCurrentIndex(index);
			model_->setCurrentTune(model_->tune(index));
			actPlayActivated();
		}
	}
}

void QompMainWin::updateTuneInfo()
{
	QModelIndex i = model_->indexForTune(model_->currentTune());
	ui->lb_artist->setText(i.data(PlayListModel::ArtistRole).toString());
	ui->lb_title->setText(i.data(PlayListModel::TitleRole).toString());

	trayIcon_->setToolTip(tr("Now playing: %1 - %2")
		.arg(i.data(PlayListModel::ArtistRole).toString(),
		     i.data(PlayListModel::TitleRole).toString()));
}
