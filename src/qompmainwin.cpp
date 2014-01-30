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
#include "qompplaylistmodel.h"
#include "options.h"
#include "pluginmanager.h"
#include "qomptrayicon.h"
#include "options/qompoptionsdlg.h"
#include "defines.h"
#include "common.h"
#include "aboutdlg.h"
#include "qomptunedownloader.h"
#include "qompplaylistdelegate.h"
#include "updateschecker.h"
#include "qompmenu.h"

#include "ui_qompmainwin.h"

#include <QTime>
#include <QCloseEvent>
#include <QFileDialog>
#include <QClipboard>
#include <QMenu>
#include <QTextStream>


QompMainWin::QompMainWin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QompMainWin),
	player_(0),
	model_(new QompPlayListModel(this)),
	trayIcon_(new QompTrayIcon(this)),
	currentState_(Stopped)
{
	ui->setupUi(this);

	ui->playList->setModel(model_);
	ui->playList->setItemDelegate(new QompPlaylistDelegate(this));

	model_->restoreState();

	ui->tb_next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->tb_prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->tb_stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->tb_clear->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
	ui->tb_load->setIcon(style()->standardIcon(QStyle::SP_DialogOpenButton));
	ui->tb_save->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
	ui->tb_open->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
	ui->tb_mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));

	ui->tb_repeatAll->setIcon((style()->standardIcon(QStyle::SP_BrowserReload)));
	ui->tb_repeatAll->setChecked(Options::instance()->getOption(OPTION_REPEAT_ALL).toBool());
	connect(ui->tb_repeatAll, SIGNAL(clicked()), SLOT(updateOptions()));

	connect(ui->tb_open, SIGNAL(clicked()), SLOT(actOpenActivated()));
	connect(ui->tb_play, SIGNAL(clicked()), SLOT(actPlayActivated()));
	connect(ui->tb_stop, SIGNAL(clicked()), SLOT(actStopActivated()));
	connect(ui->tb_clear, SIGNAL(clicked()), SLOT(actClearActivated()));
	connect(ui->tb_next, SIGNAL(clicked()), SLOT(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()), SLOT(actPrevActivated()));
	connect(ui->tb_load, SIGNAL(clicked()), SLOT(loadPlaylist()));
	connect(ui->tb_save, SIGNAL(clicked()), SLOT(savePlaylist()));
	connect(ui->tb_mute, SIGNAL(clicked(bool)), SLOT(muteButtonActivated(bool)));
	connect(ui->seekSlider, SIGNAL(valueChanged(int)), SLOT(seekSliderMoved(int)));
	connect(ui->volumeSlider, SIGNAL(valueChanged(int)), SLOT(volumeSliderMoved(int)));

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doMainContextMenu()));

	connect(ui->playList, SIGNAL(activated(QModelIndex)), SLOT(mediaActivated(QModelIndex)));
	connect(ui->playList, SIGNAL(clicked(QModelIndex)), SLOT(mediaClicked(QModelIndex)));
	connect(ui->playList, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doTrackContextMenu(QPoint)));

	//volumeChanged(1);
	setCurrentPosition(0);

	connect(model_, SIGNAL(layoutChanged()), SLOT(updateTuneInfo()));

	connect(trayIcon_, SIGNAL(trayContextMenu()), SLOT(doMainContextMenu()));
	connect(trayIcon_, SIGNAL(trayWheeled(int)), SLOT(trayWheeled(int)));

	resize(Options::instance()->getOption(OPTION_GEOMETRY_WIDTH, width()).toInt(),
		Options::instance()->getOption(OPTION_GEOMETRY_HEIGHT, height()).toInt());

	move(Options::instance()->getOption(OPTION_GEOMETRY_X, 10).toInt(),
		Options::instance()->getOption(OPTION_GEOMETRY_Y, 50).toInt());
}

QompMainWin::~QompMainWin()
{
	Options::instance()->setOption(OPTION_GEOMETRY_X, x());
	Options::instance()->setOption(OPTION_GEOMETRY_Y, y());
	Options::instance()->setOption(OPTION_GEOMETRY_HEIGHT, height());
	Options::instance()->setOption(OPTION_GEOMETRY_WIDTH, width());

	model_->saveState();

	delete player_;
	player_ = 0;
	delete ui;
}

QompPlayer *QompMainWin::player() const
{
	return player_;
}

void QompMainWin::setPlayer(QompPlayer *player)
{
	if(player_) {
		stopPlayer();
		player_->disconnect();
		player_->deleteLater();
	}

	player_ = player;
	player_->setParent(this);

	volumeChanged(player_->volume());
	setCurrentPosition(player_->position());

	connect(player_, SIGNAL(stateChanged(QompPlayer::State)), SLOT(playerStateChanged(QompPlayer::State)));
	connect(player_, SIGNAL(mediaFinished()), SLOT(playNext()));
	connect(player_, SIGNAL(currentPositionChanged(qint64)), SLOT(setCurrentPosition(qint64)));
	connect(player_, SIGNAL(mutedChanged(bool)), SLOT(updateIcons()));
	connect(player_, SIGNAL(volumeChanged(qreal)), SLOT(volumeChanged(qreal)));
	connect(player_, SIGNAL(currentTuneTotalTimeChanged(qint64)), SLOT(currentTotalTimeChanged(qint64)));

	connect(player_, SIGNAL(tuneDataUpdated(Tune*)), model_, SLOT(tuneDataUpdated(Tune*)));

	updateIcons();
	PluginManager::instance()->qompPlayerChanged(player_);
}

void QompMainWin::bringToFront()
{
	if(isMaximized())
		showMaximized();
	else
		show();
	raise();
	activateWindow();
}

void QompMainWin::actPlayActivated()
{
	if(currentState_ == Playing) {
		player_->pause();
		currentState_ = Paused;
		return;
	}

	if(!model_->rowCount())
		return;

	QModelIndex i = model_->indexForTune(model_->currentTune());
	if(!i.isValid()) {
		i = model_->index(0);
		ui->playList->setCurrentIndex(i);
		model_->setCurrentTune(model_->tune(i));
	}

	Q_ASSERT(player_);
	if(!(player_->currentTune() == model_->currentTune())) {
		player_->setTune(model_->currentTune());
		setCurrentPosition(0);
	}

	player_->play();
	currentState_ = Playing;
	updateTuneInfo();
}

void QompMainWin::updateIcons()
{
	Q_ASSERT(player_);
	switch (player_->state()) {
	case QompPlayer::StatePaused: {
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		static const QIcon pauseIco(":/icons/icons/qomp_pause.png");
		trayIcon_->setIcon(pauseIco);
		break;
	}
	case QompPlayer::StatePlaying: {
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		static const QIcon playIcon(":/icons/icons/qomp_play.png");
		trayIcon_->setIcon(playIcon);
		break;
	}
	default: {
		static const QIcon stopIco(":/icons/icons/qomp_stop.png");
		trayIcon_->setIcon(stopIco);
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		break;
	}
	}
	ui->tb_mute->setChecked(player_->isMuted());
	ui->tb_mute->setIcon(ui->tb_mute->isChecked() ?
				     style()->standardIcon(QStyle::SP_MediaVolumeMuted) :
				     style()->standardIcon(QStyle::SP_MediaVolume));
}

void QompMainWin::updateOptions()
{
	Options::instance()->setOption(OPTION_REPEAT_ALL, ui->tb_repeatAll->isChecked());
}

void QompMainWin::actPrevActivated()
{
	QModelIndex index = model_->indexForTune(model_->currentTune());
	if(index.isValid() && index.row() > 0) {
		bool play = (player_->state() == QompPlayer::StatePlaying);
		index = model_->index(index.row()-1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		if(play) {
			stopPlayer();
			actPlayActivated();
		}
	}
}

void QompMainWin::actNextActivated()
{
	QModelIndex index = model_->indexForTune(model_->currentTune());
	if(index.isValid() && index.row() < model_->rowCount()-1) {
		Q_ASSERT(player_);
		bool play = (player_->state() == QompPlayer::StatePlaying);
		index = model_->index(index.row()+1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		if(play) {
			stopPlayer();
			actPlayActivated();
		}

	}
}

void QompMainWin::actStopActivated()
{
	Q_ASSERT(player_);
	stopPlayer();
	ui->lb_busy->stop();
	QModelIndex index = ui->playList->currentIndex();
	if(index.isValid())
		model_->setCurrentTune(model_->tune(index));
	ui->lb_artist->setText("");
	ui->lb_title->setText("");
	trayIcon_->setToolTip("");
}

void QompMainWin::actOpenActivated()
{
	QompGetTunesMenu m;
	connect(&m, SIGNAL(tunes(TuneList)), SLOT(tunes(TuneList)));
	m.move(QCursor::pos());
	m.exec();
}

void QompMainWin::actClearActivated()
{
	Q_ASSERT(player_);
	QMenu m;
	QList<QAction*> acts;
	acts << new QAction(tr("Remove All"), &m)
	     << new QAction(tr("Remove Selected"), &m);
	m.addActions(acts);
	m.move(QCursor::pos());
	int x = acts.indexOf(m.exec());
	if(x == 0) {
		stopPlayer();
		model_->clear();
	}
	else if(x == 1) {
		bool removingCurrent = false;
		TuneList list;
		foreach(const QModelIndex& index, ui->playList->selectionModel()->selectedIndexes()) {
			Tune* t = model_->tune(index);
			if(t == model_->currentTune())
				removingCurrent = true;
			list << t;
		}
		foreach(Tune* tune, list)
			model_->removeTune(tune);

		if(removingCurrent) {
			stopPlayer();
			model_->setCurrentTune(model_->tune(model_->index(0,0)));
		}
		ui->playList->setCurrentIndex(model_->indexForTune(model_->currentTune()));

	}
	updateTuneInfo();
}

void QompMainWin::muteButtonActivated(bool b)
{
	Q_ASSERT(player_);
	if(player_->isMuted() !=b) {
		player_->setMute(b);
		updateIcons();
	}
}

void QompMainWin::volumeSliderMoved(int vol)
{
	Q_ASSERT(player_);
	qreal newVol = qreal(vol)/1000;
	player_->setVolume(newVol);
}

void QompMainWin::volumeChanged(qreal vol)
{
	ui->volumeSlider->blockSignals(true);
	ui->volumeSlider->setValue(vol*1000);
	ui->volumeSlider->blockSignals(false);
}

void QompMainWin::seekSliderMoved(int pos)
{
	Q_ASSERT(player_);
	player_->setPosition(pos);
}

void QompMainWin::mediaActivated(const QModelIndex &index)
{
	model_->setCurrentTune(model_->tune(index));
	stopPlayer();
	actPlayActivated();
}

void QompMainWin::mediaClicked(const QModelIndex &index)
{
	Q_ASSERT(player_);

	if(currentState_ == Stopped)
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
	Tune* tune = model_->tune(index);
	QMenu menu;
	QList<QAction*> acts;
	acts << new QAction(tr("Play/Pause"), &menu);
	acts << new QAction(tr("Remove"), &menu);
	if(!tune->url.isEmpty()) {
		acts << new QAction(tr("Copy URL"), &menu);		
	}
	if(tune->canSave()) {
		acts << new QAction(tr("Save File"), &menu);
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
		qApp->clipboard()->setText(tune->getUrl().toString());
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
	QompMainMenu m;
	m.move(QCursor::pos());
	connect(&m, SIGNAL(actToggleVisibility()), SLOT(toggleVisibility()));
	connect(&m, SIGNAL(actCheckUpdates()), SLOT(checkForUpdates()));
	connect(&m, SIGNAL(actAbout()), SLOT(aboutQomp()));
	connect(&m, SIGNAL(actDoOptions()), SLOT(doOptions()));
	connect(&m, SIGNAL(tunes(TuneList)), SLOT(tunes(TuneList)));
	connect(&m, SIGNAL(actExit()), SIGNAL(exit()));

	m.exec();
}

void QompMainWin::playerStateChanged(QompPlayer::State state)
{
	updateIcons();
	ui->lb_busy->stop();
	if(currentState_ == Stopped)
		return;

	switch(state) {
	case QompPlayer::StateError:
		playNext();
		break;
	case QompPlayer::StateBuffering:
		ui->lb_busy->changeText(tr("Buffering"));
		ui->lb_busy->start();
		break;
	case QompPlayer::StateLoading:
		ui->lb_busy->changeText(tr("Loading"));
		ui->lb_busy->start();
		break;
	default:
		break;
	}
}

void QompMainWin::setCurrentPosition(qint64 ms)
{
	ui->lcd->display(Qomp::durationMiliSecondsToString(ms));
	ui->seekSlider->blockSignals(true);
	ui->seekSlider->setValue(ms);
	ui->seekSlider->blockSignals(false);
}

void QompMainWin::currentTotalTimeChanged(qint64 ms)
{
	Q_ASSERT(player_);
	ui->seekSlider->setMaximum(ms);

	if(ms == -1 || ms == 0)
		return;
	player_->currentTune()->duration = Qomp::durationMiliSecondsToString(ms);
	model_->tuneDataUpdated(player_->currentTune());
}

void QompMainWin::playNext()
{
	setCurrentPosition(0);
	if(currentState_ == Stopped)
		return;

	if(model_->indexForTune(model_->currentTune()).row() == model_->rowCount()-1) {
		if(ui->tb_repeatAll->isChecked()) {
			const QModelIndex ind = model_->index(0);
			model_->setCurrentTune(model_->tune(ind));
			ui->playList->setCurrentIndex(ind);
			stopPlayer();
			actPlayActivated();

		}
		else {
			actStopActivated();
			model_->setCurrentTune((Tune*)Tune::emptyTune());
		}
	}
	else {
		QModelIndex index = model_->indexForTune(model_->currentTune());
		index = model_->index(index.row()+1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		stopPlayer();
		actPlayActivated();
	}
}

void QompMainWin::doOptions()
{
	QompOptionsDlg dlg(this);
	dlg.exec();
}

void QompMainWin::loadPlaylist()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Select Playlist"),
			Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), "*.qomp");
	if(!file.isEmpty()) {
		Options::instance()->setOption(LAST_DIR, file);
		model_->loadTunes(file);
	}
}

void QompMainWin::savePlaylist()
{
	QString file = QFileDialog::getSaveFileName(this, tr("Save Playlist"),
			Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), "*.qomp");
	if(!file.isEmpty()) {
		Options::instance()->setOption(LAST_DIR, file);
		model_->saveTunes(file);
	}
}

void QompMainWin::toggleVisibility()
{
	bool b = isHidden();
	if(b) {
		bringToFront();
	}
	else
		hide();
}

void QompMainWin::checkForUpdates()
{
	new UpdatesChecker(this);
}

void QompMainWin::aboutQomp()
{
	new AboutDlg(this);
}

void QompMainWin::trayActivated(Qt::MouseButton b)
{
	if(b == Qt::RightButton) {
		doMainContextMenu();
	}
	else if(b == Qt::MidButton) {
		if(player_->state() == QompPlayer::StatePlaying) {
			player_->pause();
			currentState_ = Paused;
		}
		else if(player_->state() == QompPlayer::StatePaused) {
			player_->play();
			currentState_ = Playing;
		}
	}
}

void QompMainWin::trayWheeled(int delta)
{
	qreal vol = player_->volume();
	vol += qreal(delta)/1000;
	player_->setVolume(vol);
}

void QompMainWin::closeEvent(QCloseEvent *e)
{
	if(Options::instance()->getOption(OPTION_HIDE_ON_CLOSE).toBool()) {
		hide();
		e->ignore();
	}
	else {
		emit exit();
		e->accept();
	}
}

void QompMainWin::changeEvent(QEvent *e)
{
	if(e->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}
	QMainWindow::changeEvent(e);
}

void QompMainWin::tunes(const TuneList &list)
{
	if(!list.isEmpty()) {
		model_->addTunes(list);

		Q_ASSERT(player_);
		if(player_->state() != QompPlayer::StatePaused
			&& player_->state() != QompPlayer::StatePlaying)
		{
			QModelIndex index = model_->indexForTune(list.first());
			ui->playList->setCurrentIndex(index);
			model_->setCurrentTune(model_->tune(index));
			actPlayActivated();
		}

		if(Options::instance()->getOption(OPTION_UPDATE_METADATA).toBool())
		{
			player_->resolveMetadata(list);
		}
	}
}

void QompMainWin::stopPlayer()
{
	player_->stop();
	setCurrentPosition(0);
	currentState_ = Stopped;
}

void QompMainWin::updateTuneInfo()
{
	QModelIndex i = model_->indexForTune(model_->currentTune());
	ui->lb_artist->setText(i.data(QompPlayListModel::ArtistRole).toString());
	ui->lb_title->setText(i.data(QompPlayListModel::TitleRole).toString());

	trayIcon_->setToolTip(tr("Now playing: %1 - %2")
		.arg(i.data(QompPlayListModel::ArtistRole).toString(),
		     i.data(QompPlayListModel::TitleRole).toString()));
}
