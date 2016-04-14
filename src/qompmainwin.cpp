/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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
#include "qomptrayicon.h"
#include "defines.h"
#include "qompplaylistdelegate.h"
#include "qompmenu.h"
#include "tune.h"
#include "qompactionslist.h"

#include "ui_qompmainwin.h"

#include <QTime>
#include <QMainWindow>
#include <QFileDialog>

class QompMainWin::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QompMainWin *p);
	~Private();

	void connectActions();
	void connectMainMenu();
	void setupPlaylist();

	void saveWindowState();
	void restoreWindowState();
	void updateVolumSliderToolTip();
	void bringToFront();

public slots:
	void actOpenActivated();
	void actClearActivated();
	void volumeSliderMoved(int);
	void doTrackContextMenu(const QPoint& p);
	void doMainContextMenu();

	void updateTuneInfo(Tune *tune);
	void updateIcons(Qomp::State state);

	void trayWheeled(int delta);

	void updateOptions();
	void removeSelectedIndexes();
	void totalDurationChanged(uint time);

	void actSavePlaylist();
	void actLoadPlaylist();
	void actDownloadTune(Tune *tune);

	void updateShortcuts();

public:
	Ui::QompMainWin *ui;
	QMainWindow*  mainWin_;
	QompTrayIcon* trayIcon_;
	QompMainMenu* mainMenu_;
	QompMainWin*  parentWin_;
	QAction* actClearPlaylist_;
};

QompMainWin::Private::Private(QompMainWin *p) :
	QObject(p),
	ui(new Ui::QompMainWin),
	mainWin_(new QMainWindow),
	trayIcon_(new QompTrayIcon(p)),
	mainMenu_(new QompMainMenu(mainWin_)),
	parentWin_(p),
	actClearPlaylist_(0)
{
	ui->setupUi(mainWin_);
	connectActions();
	connectMainMenu();
	setupPlaylist();
	updateShortcuts();

	ui->tb_repeatAll->setChecked(Options::instance()->getOption(OPTION_REPEAT_ALL).toBool());
	ui->tb_shuffle->setChecked(Options::instance()->getOption(OPTION_SHUFFLE).toBool());

	trayIcon_->setContextMenu(mainMenu_);

	connect(ui->seekSlider, SIGNAL(valueChanged(int)), parentWin_, SIGNAL(seekSliderMoved(int)));
	connect(ui->volumeSlider, SIGNAL(valueChanged(int)), SLOT(volumeSliderMoved(int)));
	connect(mainWin_, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doMainContextMenu()));
	connect(trayIcon_, SIGNAL(trayWheeled(int)), SLOT(trayWheeled(int)));

	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(updateShortcuts()));

	restoreWindowState();
	totalDurationChanged(0);

	mainWin_->installEventFilter(parentWin_);
}

QompMainWin::Private::~Private()
{
	saveWindowState();

	delete ui;
}

void QompMainWin::Private::connectMainMenu()
{
	connect(mainMenu_, SIGNAL(actToggleVisibility()), parentWin_, SLOT(toggleVisibility()));
	connect(mainMenu_, SIGNAL(actCheckUpdates()), parentWin_, SIGNAL(checkForUpdates()));
	connect(mainMenu_, SIGNAL(actAbout()), parentWin_, SIGNAL(aboutQomp()));
	connect(mainMenu_, SIGNAL(actDoOptions()), parentWin_, SIGNAL(doOptions()));
	connect(mainMenu_, SIGNAL(tunes(QList<Tune*>)), parentWin_, SIGNAL(tunes(QList<Tune*>)));
	connect(mainMenu_, SIGNAL(actExit()), qApp, SLOT(quit()));
	connect(mainMenu_, SIGNAL(actReportBug()), parentWin_, SIGNAL(bugReport()));
}

void QompMainWin::Private::connectActions()
{
	connect(ui->tb_repeatAll,SIGNAL(clicked()), SLOT(updateOptions()));
	connect(ui->tb_shuffle,	 SIGNAL(clicked()), SLOT(updateOptions()));
	connect(ui->tb_open,	 SIGNAL(clicked()), SLOT(actOpenActivated()));
	connect(ui->tb_load,	 SIGNAL(clicked()), SLOT(actLoadPlaylist()));
	connect(ui->tb_save,	 SIGNAL(clicked()), SLOT(actSavePlaylist()));
	connect(ui->tb_clear,	 SIGNAL(clicked()), SLOT(actClearActivated()));

	connect(ui->tb_play, SIGNAL(clicked()),     parentWin_, SIGNAL(actPlayActivated()));
	connect(ui->tb_stop, SIGNAL(clicked()),     parentWin_, SIGNAL(actStopActivated()));
	connect(ui->tb_next, SIGNAL(clicked()),     parentWin_, SIGNAL(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()),	    parentWin_, SIGNAL(actPrevActivated()));
	connect(ui->tb_mute, SIGNAL(clicked(bool)), parentWin_, SIGNAL(actMuteActivated(bool)));
	connect(ui->tbOptions, SIGNAL(clicked()),   parentWin_, SIGNAL(doOptions()));

	actClearPlaylist_ = new QAction(mainWin_);
	connect(actClearPlaylist_, SIGNAL(triggered()), parentWin_, SIGNAL(clearPlaylist()));
	mainWin_->addAction(actClearPlaylist_);
}

void QompMainWin::Private::setupPlaylist()
{
	ui->playList->setItemDelegate(new QompPlaylistDelegate(this));

	connect(ui->playList, SIGNAL(activated(QModelIndex)), parentWin_, SIGNAL(mediaActivated(QModelIndex)));
	connect(ui->playList, SIGNAL(clicked(QModelIndex)), parentWin_, SIGNAL(mediaClicked(QModelIndex)));
	connect(ui->playList, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doTrackContextMenu(QPoint)));
}


void QompMainWin::Private::saveWindowState()
{
	Options::instance()->setOption(OPTION_GEOMETRY_X, mainWin_->x());
	Options::instance()->setOption(OPTION_GEOMETRY_Y, mainWin_->y());
	Options::instance()->setOption(OPTION_GEOMETRY_HEIGHT, mainWin_->height());
	Options::instance()->setOption(OPTION_GEOMETRY_WIDTH, mainWin_->width());
}

void QompMainWin::Private::restoreWindowState()
{
	mainWin_->resize(Options::instance()->getOption(OPTION_GEOMETRY_WIDTH, mainWin_->width()).toInt(),
		Options::instance()->getOption(OPTION_GEOMETRY_HEIGHT, mainWin_->height()).toInt());

	mainWin_->move(Options::instance()->getOption(OPTION_GEOMETRY_X, 10).toInt(),
		Options::instance()->getOption(OPTION_GEOMETRY_Y, 50).toInt());
}


void QompMainWin::Private::updateVolumSliderToolTip()
{
	ui->volumeSlider->setToolTip(QString("%1 %").arg(QString::number(ui->volumeSlider->value() / 10)));
}

void QompMainWin::Private::bringToFront()
{
	if(mainWin_->isMaximized())
		mainWin_->showMaximized();
	else
		mainWin_->show();
	mainWin_->raise();
	mainWin_->activateWindow();
}

void QompMainWin::Private::actOpenActivated()
{
	QompGetTunesMenu m(mainWin_);
	connect(&m, SIGNAL(tunes(QList<Tune*>)), parentWin_, SIGNAL(tunes(QList<Tune*>)));

	QPoint p = ui->tb_open->pos();
	p.setY(p.y() + ui->tb_open->height()/2);
	m.exec(mainWin_->mapToGlobal(p));
}

void QompMainWin::Private::actClearActivated()
{
	QompRemoveTunesMenu m(mainWin_);
	connect(&m, SIGNAL(removeAll()), parentWin_, SIGNAL(clearPlaylist()));
	connect(&m, SIGNAL(removeSelected()), SLOT(removeSelectedIndexes()));

	QPoint p = ui->tb_clear->pos();
	p.setY(p.y() + ui->tb_clear->height()/2);
	m.exec(mainWin_->mapToGlobal(p));

	updateTuneInfo(parentWin_->model_->currentTune());
}

void QompMainWin::Private::volumeSliderMoved(int vol)
{
	qreal newVol = qreal(vol)/1000;
	updateVolumSliderToolTip();
	emit parentWin_->volumeSliderMoved(newVol);
}

void QompMainWin::Private::doMainContextMenu()
{
	mainMenu_->exec(QCursor::pos());
}

void QompMainWin::Private::doTrackContextMenu(const QPoint& val)
{
	QModelIndex index = ui->playList->indexAt(val);
	if(!index.isValid())
		return;

	ui->playList->setCurrentIndex(index);
	Tune* tune = parentWin_->model_->tune(index);
	QompTrackMenu menu(tune, mainWin_);
	connect(&menu, SIGNAL(saveTune(Tune*)), SLOT(actDownloadTune(Tune*)));
	connect(&menu, SIGNAL(togglePlayState(Tune*)),	parentWin_, SIGNAL(toggleTuneState(Tune*)));
	connect(&menu, SIGNAL(removeTune(Tune*)),	parentWin_, SIGNAL(removeTune(Tune*)));
	connect(&menu, SIGNAL(copyUrl(Tune*)),		parentWin_, SIGNAL(copyUrl(Tune*)));

	menu.exec(QCursor::pos());
}

void QompMainWin::Private::updateTuneInfo(Tune* tune)
{
	QModelIndex i = parentWin_->model_->indexForTune(tune);
	if(i.isValid() && (parentWin_->currentState_ == Qomp::StatePaused || parentWin_->currentState_ == Qomp::StatePlaying)) {
		ui->lb_artist->setText(i.data(QompPlayListModel::ArtistRole).toString());
		ui->lb_title->setText(i.data(QompPlayListModel::TitleRole).toString());

		trayIcon_->setToolTip(tr("Now playing: %1 - %2")
				      .arg(i.data(QompPlayListModel::ArtistRole).toString(),
					   i.data(QompPlayListModel::TitleRole).toString()));
	} else {
		ui->lb_artist->setText("");
		ui->lb_title->setText("");
		trayIcon_->setToolTip("");
	}
}

void QompMainWin::Private::updateIcons(Qomp::State state)
{
	switch (state) {
	case Qomp::StatePaused: {
		ui->tb_play->setIcon(QIcon(":/icons/play"));
		static const QIcon pauseIco(":/icons/icons/qomp_pause.png");
		trayIcon_->setIcon(pauseIco);
		break;
	}
	case Qomp::StatePlaying: {
		ui->tb_play->setIcon(QIcon(":/icons/pause"));
		static const QIcon playIcon(":/icons/icons/qomp_play.png");
		trayIcon_->setIcon(playIcon);
		break;
	}
	default: {
		static const QIcon stopIco(":/icons/icons/qomp_stop.png");
		trayIcon_->setIcon(stopIco);
		ui->tb_play->setIcon(QIcon(":/icons/play"));
		break;
	}
	}
}

void QompMainWin::Private::trayWheeled(int delta)
{
	int vol = ui->volumeSlider->value();
	int min = ui->volumeSlider->minimum();
	int max = ui->volumeSlider->maximum();
	vol += delta;
	vol = (vol < min) ? min : (vol >= max) ? max : vol;
	ui->volumeSlider->setValue(vol);
}

void QompMainWin::Private::updateOptions()
{
	Options::instance()->setOption(OPTION_REPEAT_ALL, ui->tb_repeatAll->isChecked());
	Options::instance()->setOption(OPTION_SHUFFLE, ui->tb_shuffle->isChecked());
}

void QompMainWin::Private::removeSelectedIndexes()
{
	emit parentWin_->removeSelected(ui->playList->selectionModel()->selectedIndexes());
}


void QompMainWin::Private::totalDurationChanged(uint time)
{
	QTime t = QTime(0,0,0,0).addSecs(time);
	ui->lb_playtime->setText(t.toString("hh:mm:ss"));
}

void QompMainWin::Private::actSavePlaylist()
{
	QFileDialog f(0, tr("Select Playlist"),
		      Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), tr("qomp playlist (*.qomp)"));
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptSave);
	if (f.exec() == QFileDialog::Accepted) {
		QStringList files = f.selectedFiles();
		if(!files.isEmpty()) {
			parentWin_->savePlaylist(files.first());
		}
	}
}

void QompMainWin::Private::actLoadPlaylist()
{
	QFileDialog f(0,tr("Select Playlist"),
		      Options::instance()->getOption(LAST_DIR, QDir::homePath()).toString(), tr("qomp playlist (*.qomp)"));
	f.setFileMode(QFileDialog::ExistingFile);
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptOpen);
	if (f.exec() == QFileDialog::Accepted) {
		QStringList files = f.selectedFiles();
		if(!files.isEmpty()) {
			parentWin_->loadPlaylist(files.first());
		}
	}
}

void QompMainWin::Private::actDownloadTune(Tune *tune)
{
	QFileDialog f(0,tr("Select directory"),Options::instance()->getOption(LAST_SAVE_DIR, QDir::homePath()).toString());
	f.setFileMode(QFileDialog::Directory);
	f.setViewMode(QFileDialog::List);
	if (f.exec() == QFileDialog::Accepted) {
		QStringList dir = f.selectedFiles();
		if(dir.isEmpty())
			return;

		Options::instance()->setOption(LAST_SAVE_DIR, dir.first());
		parentWin_->downloadTune(tune, dir.first());
	}
}

void QompMainWin::Private::updateShortcuts()
{
	QompActionsList* a = QompActionsList::instance();

	ui->tb_play->setShortcut(a->getShortcut(QompActionsList::ActPlay));
	ui->tb_next->setShortcut(a->getShortcut(QompActionsList::ActNext));
	ui->tb_prev->setShortcut(a->getShortcut(QompActionsList::ActPrevious));
	ui->tb_stop->setShortcut(a->getShortcut(QompActionsList::ActStop));
	ui->tb_open->setShortcut(a->getShortcut(QompActionsList::ActOpen));
	ui->tb_mute->setShortcut(a->getShortcut(QompActionsList::ActMute));
	ui->tb_repeatAll->setShortcut(a->getShortcut(QompActionsList::ActRepeatAll));
	ui->tb_shuffle->setShortcut(a->getShortcut(QompActionsList::ActShuffle));
	ui->tbOptions->setShortcut(a->getShortcut(QompActionsList::ActSettings));
	actClearPlaylist_->setShortcut(a->getShortcut(QompActionsList::ActClearPlaylist));
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
	d->ui->playList->setModel(model_);
	connect(model_, SIGNAL(currentTuneChanged(Tune*)), d, SLOT(updateTuneInfo(Tune*)));
	connect(model_, SIGNAL(totalTimeChanged(uint)), d, SLOT(totalDurationChanged(uint)));
}

void QompMainWin::setMuteState(bool mute)
{
	d->ui->tb_mute->setChecked(mute);
	d->ui->tb_mute->setIcon(d->ui->tb_mute->isChecked() ?
				     QIcon(":/icons/mute") :
				     QIcon(":/icons/volume"));
}

void QompMainWin::volumeChanged(qreal vol)
{
	d->ui->volumeSlider->blockSignals(true);
	d->ui->volumeSlider->setValue(vol*1000);
	d->updateVolumSliderToolTip();
	d->ui->volumeSlider->blockSignals(false);
}

void QompMainWin::playerStateChanged(Qomp::State state)
{
	if(currentState_ == state)
		return;

	d->updateIcons(state);
	currentState_ = state;

	switch(state) {
	case Qomp::StateBuffering:
		d->ui->lb_busy->changeText(tr("Buffering"));
		d->ui->lb_busy->start();
		break;
	case Qomp::StateLoading:
		d->ui->lb_busy->changeText(tr("Loading"));
		d->ui->lb_busy->start();
		break;
	default:
		d->ui->lb_busy->stop();
		break;
	}
	d->updateTuneInfo(model_->currentTune());
}

void QompMainWin::hide()
{
	d->mainWin_->hide();
}

void QompMainWin::show()
{
	d->mainWin_->show();
}

bool QompMainWin::eventFilter(QObject *o, QEvent *e)
{
	if(o == d->mainWin_) {
		if(e->type() == QEvent::LanguageChange) {
			d->ui->retranslateUi(d->mainWin_);
		}
		else if(e->type() == QEvent::Close) {
			if(Options::instance()->getOption(OPTION_HIDE_ON_CLOSE).toBool()) {
				hide();
				e->ignore();
			}
			else {
				qApp->quit();
				e->accept();
			}
		}
	}

	return QObject::eventFilter(o,e);
}

void QompMainWin::setCurrentPosition(qint64 ms)
{
	d->ui->lcd->display(Qomp::durationMiliSecondsToString(ms));
	d->ui->seekSlider->blockSignals(true);
	d->ui->seekSlider->setValue(ms);
	d->ui->seekSlider->blockSignals(false);
}

void QompMainWin::currentTotalTimeChanged(qint64 ms)
{
	d->ui->seekSlider->setMaximum(ms);

	if(ms == -1 || ms == 0)
		return;
	model_->currentTune()->duration = Qomp::durationMiliSecondsToString(ms);
	model_->tuneDataUpdated(model_->currentTune());
}

void QompMainWin::toggleVisibility()
{
	bool b = d->mainWin_->isHidden();
	if(b) {
		d->bringToFront();
	}
	else
		hide();
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


#include "qompmainwin.moc"
