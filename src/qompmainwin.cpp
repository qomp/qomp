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

#include "qompmainwin.h"
#include "qompplaylistmodel.h"
#include "options.h"
#include "qomptrayicon.h"
#include "defines.h"
#include "qompplaylistdelegate.h"
#include "qompmenu.h"
#include "tune.h"
#include "qompactionslist.h"
#include "thememanager.h"

#include "ui_qompmainwin.h"

#include <QTime>
#include <QMainWindow>
#include <QFileDialog>
#ifdef Q_OS_WIN
#include <QtWinExtras>
#endif

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
	void updateProgress(Qomp::State state);

	void trayWheeled(int delta);

	void updateOptions();
	void removeSelectedIndexes();
	void totalDurationChanged(uint time);

	void actSavePlaylist();
	void actLoadPlaylist();
	void actDownloadTune(const QModelIndexList& list);

	void updateShortcuts();	
	void updateShuffleIcon();
	void updateMuteIcon();
	void updatePlaylistIcon();
	void updateIcons();

	void togglePlaylistVisibility();
	void showPlaylist();
	void hidePlaylist();

	void initTaskBar();

public:
	Ui::QompMainWin *ui;
	QMainWindow*  mainWin_;
	QompTrayIcon* trayIcon_;
	QompMainMenu* mainMenu_;
	QompMainWin*  parentWin_;
	QAction* actClearPlaylist_;
	QAction* actRemoveSelected_;
#ifdef Q_OS_WIN
	QWinTaskbarButton *winTaskBar_;
#endif
};


QompMainWin::Private::Private(QompMainWin *p) :
	QObject(p),
	ui(new Ui::QompMainWin),
	mainWin_(new QMainWindow),
	trayIcon_(new QompTrayIcon(p)),
	mainMenu_(new QompMainMenu(mainWin_)),
	parentWin_(p),
	actClearPlaylist_(nullptr),
	actRemoveSelected_(nullptr)
#ifdef Q_OS_WIN
	, winTaskBar_(nullptr)
#endif
{
	ui->setupUi(mainWin_);
	connectActions();
	connectMainMenu();
	setupPlaylist();
	updateShortcuts();
	updateIcons();

	ui->tb_repeatAll->setChecked(Options::instance()->getOption(OPTION_REPEAT_ALL).toBool());
	ui->tb_shuffle->setChecked(Options::instance()->getOption(OPTION_SHUFFLE).toBool());
	updateShuffleIcon();

#if defined(HAVE_X11) && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
	QMenu *menu = new QMenu(mainWin_);
	menu->addAction(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/toggle")), tr("Toggle Visibility"), parentWin_, SLOT(toggleVisibility()))->setParent(menu);
	menu->addSeparator();
	menu->addAction(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/close")), tr("Exit"), qApp, SLOT(quit()))->setParent(menu);

	trayIcon_->setContextMenu(menu);
#else
	trayIcon_->setContextMenu(mainMenu_);
#endif

#ifdef Q_OS_WIN
	if (QSysInfo::windowsVersion() >= QSysInfo::WV_WINDOWS7) {
		winTaskBar_ = new QWinTaskbarButton(this);
		winTaskBar_->progress()->setMinimum(0);
	}
#endif

	connect(ui->seekSlider, &QSlider::valueChanged, parentWin_, &QompMainWin::seekSliderMoved);
	connect(ui->volumeSlider, &QSlider::valueChanged, this, &Private::volumeSliderMoved);
	connect(mainWin_, SIGNAL(customContextMenuRequested(QPoint)), SLOT(doMainContextMenu()));
	connect(trayIcon_, &QompTrayIcon::trayWheeled, this, &Private::trayWheeled);
	connect(ui->tb_showPlaylist, SIGNAL(clicked(bool)), SLOT(togglePlaylistVisibility()));
//	if(winTaskBar_)
//		connect(trayIcon_, &QompTrayIcon::iconChanged, winTaskBar_, &QWinTaskbarButton::setOverlayIcon);

	connect(Options::instance(), &Options::updateOptions, this, &Private::updateShortcuts);

	restoreWindowState();

	totalDurationChanged(0);

	mainWin_->installEventFilter(parentWin_);
}

QompMainWin::Private::~Private()
{
	saveWindowState();

	delete ui;
}

void QompMainWin::Private::updateShuffleIcon()
{
	ui->tb_shuffle->setIcon(ui->tb_shuffle->isChecked() ?
				     QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/random")) :
				     QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/linear")));
}

void QompMainWin::Private::updateMuteIcon()
{
	ui->tb_mute->setIcon(ui->tb_mute->isChecked() ?
				     QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/mute")) :
				     QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/volume")));
}

void QompMainWin::Private::updatePlaylistIcon()
{
	ui->tb_showPlaylist->setIcon(Options::instance()->getOption(OPTION_PLAYLIST_VISIBLE).toBool() ?
				     QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/arrow-down")) :
					     QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/arrow-up")));
}

void QompMainWin::Private::updateIcons()
{
	ui->tb_clear->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/delete")));
	ui->tb_prev->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/prev")));
	ui->tb_next->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/next")));
	ui->tb_stop->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/stop")));
	ui->tb_repeatAll->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/repeat")));
	ui->tb_load->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/load")));
	ui->tb_save->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/save")));
	ui->tb_open->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/add")));
	ui->tb_options->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/options")));
}

void QompMainWin::Private::togglePlaylistVisibility()
{
	bool vis;
	if (ui->playList->isVisible()) {
		const int h = ui->playList->height();
		Options::instance()->setOption(OPTION_PLAYLIST_HEIGHT, h);
		hidePlaylist();
		vis = false;
	}
	else {
		showPlaylist();
		vis = true;
	}

	Options::instance()->setOption(OPTION_PLAYLIST_VISIBLE, vis);
	updatePlaylistIcon();
}

void QompMainWin::Private::showPlaylist()
{
	if (ui->playList->isVisible())
		return;

	const int h = Options::instance()->getOption(OPTION_PLAYLIST_HEIGHT).toInt();
	mainWin_->setMaximumHeight(QWIDGETSIZE_MAX);
	mainWin_->resize( mainWin_->width(), mainWin_->height() + h );
	ui->playList->show();
	Qomp::forceUpdate(mainWin_);
}

void QompMainWin::Private::hidePlaylist()
{
	ui->playList->hide();
	Qomp::forceUpdate(mainWin_);
	mainWin_->resize( mainWin_->width(), 0 );
	mainWin_->setMaximumHeight(mainWin_->height());
}

void QompMainWin::Private::initTaskBar()
{
#ifdef Q_OS_WIN
	static bool inited = false;
	if(winTaskBar_ && !inited) {
		winTaskBar_->setWindow(mainWin_->windowHandle());
		inited = true;
	}
#endif
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
	connect(ui->tb_shuffle,	 SIGNAL(clicked()), SLOT(updateShuffleIcon()));
	connect(ui->tb_open,	 SIGNAL(clicked()), SLOT(actOpenActivated()));
	connect(ui->tb_load,	 SIGNAL(clicked()), SLOT(actLoadPlaylist()));
	connect(ui->tb_save,	 SIGNAL(clicked()), SLOT(actSavePlaylist()));
	connect(ui->tb_clear,	 SIGNAL(clicked()), SLOT(actClearActivated()));

	connect(ui->tb_play, SIGNAL(clicked()),     parentWin_, SIGNAL(actPlayActivated()));
	connect(ui->tb_stop, SIGNAL(clicked()),     parentWin_, SIGNAL(actStopActivated()));
	connect(ui->tb_next, SIGNAL(clicked()),     parentWin_, SIGNAL(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()),	    parentWin_, SIGNAL(actPrevActivated()));
	connect(ui->tb_mute, SIGNAL(clicked(bool)), parentWin_, SIGNAL(actMuteActivated(bool)));
	connect(ui->tb_options, SIGNAL(clicked()),   parentWin_, SIGNAL(doOptions()));

	actClearPlaylist_ = new QAction(mainWin_);
	connect(actClearPlaylist_, &QAction::triggered, parentWin_, &QompMainWin::clearPlaylist);
	mainWin_->addAction(actClearPlaylist_);

	actRemoveSelected_ = new QAction(mainWin_);
	connect(actRemoveSelected_, &QAction::triggered, this, &Private::removeSelectedIndexes);
	mainWin_->addAction(actRemoveSelected_);
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
	if (!Options::instance()->getOption(OPTION_PLAYLIST_VISIBLE).toBool())
		hidePlaylist();

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
		mainWin_->showNormal();

	mainWin_->raise();
	mainWin_->activateWindow();

	initTaskBar();
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

	auto list = ui->playList->selectionModel()->selectedIndexes();

//	ui->playList->setCurrentIndex(index);
//	Tune* tune = parentWin_->model_->tune(index);
	QompTrackMenu menu(list, mainWin_);
	connect(&menu, &QompTrackMenu::saveTune,	this,	    &Private::actDownloadTune);
	connect(&menu, &QompTrackMenu::togglePlayState,	parentWin_, &QompMainWin::toggleTuneState);
	connect(&menu, &QompTrackMenu::removeTune,	parentWin_, &QompMainWin::removeSelected);
	connect(&menu, &QompTrackMenu::copyUrl,		parentWin_, &QompMainWin::copyUrl);

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
		ui->tb_play->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/play")));
		static const QIcon pauseIco(":/icons/icons/qomp_pause.png");
		trayIcon_->setIcon(pauseIco);
		break;
	}
	case Qomp::StatePlaying: {
		ui->tb_play->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/pause")));
		static const QIcon playIcon(":/icons/icons/qomp_play.png");
		trayIcon_->setIcon(playIcon);
		break;
	}
	default: {
		static const QIcon stopIco(":/icons/icons/qomp_stop.png");
		trayIcon_->setIcon(stopIco);
		ui->tb_play->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/play")));
		break;
	}
	}
}

void QompMainWin::Private::updateProgress(Qomp::State state)
{
#ifdef Q_OS_WIN
	if(!winTaskBar_)
		return;

	switch (state) {
	case Qomp::StatePaused:
		winTaskBar_->progress()->setVisible(true);
		winTaskBar_->progress()->pause();
		break;
	case Qomp::StatePlaying:
		winTaskBar_->progress()->setVisible(true);
		winTaskBar_->progress()->resume();
		break;
	default:
		winTaskBar_->progress()->setVisible(false);
		winTaskBar_->progress()->stop();
		break;
	}
#endif
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

void QompMainWin::Private::actDownloadTune(const QModelIndexList &list)
{
	QFileDialog f(0,tr("Select directory"),Options::instance()->getOption(LAST_SAVE_DIR, QDir::homePath()).toString());
	f.setFileMode(QFileDialog::Directory);
	f.setViewMode(QFileDialog::List);
	if (f.exec() == QFileDialog::Accepted) {
		QStringList dir = f.selectedFiles();
		if(dir.isEmpty())
			return;

		Options::instance()->setOption(LAST_SAVE_DIR, dir.first());
		for(const QModelIndex& i: list){
			emit parentWin_->downloadTune(i.data(QompPlayListModel::TuneRole).value<Tune*>(),
						      dir.first());
		}
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
	ui->tb_options->setShortcut(a->getShortcut(QompActionsList::ActSettings));
	actClearPlaylist_->setShortcut(a->getShortcut(QompActionsList::ActClearPlaylist));
	actRemoveSelected_->setShortcut(a->getShortcut(QompActionsList::ActRemoveTune));
}




QompMainWin::QompMainWin(QObject *parent) :
	QObject(parent),
	d(new Private(this)),
	model_(0),	
	currentState_(Qomp::StateUnknown)
{
	d->trayIcon_->show();
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
	connect(model_, &QompPlayListModel::currentTuneChanged, d, &QompMainWin::Private::updateTuneInfo);
	connect(model_, &QompPlayListModel::totalTimeChanged, d, &QompMainWin::Private::totalDurationChanged);
}

void QompMainWin::setMuteState(bool mute)
{
	d->ui->tb_mute->setChecked(mute);
	d->updateMuteIcon();
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
	d->updateProgress(state);
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
	d->bringToFront();
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
#ifdef Q_OS_WIN
	if(d->winTaskBar_)
		d->winTaskBar_->progress()->setValue(ms / 1000);
#endif
}

void QompMainWin::currentTotalTimeChanged(qint64 ms)
{
	d->ui->seekSlider->setMaximum(ms);
#ifdef Q_OS_WIN
	if(d->winTaskBar_)
		d->winTaskBar_->progress()->setMaximum(ms / 1000);
#endif

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

void QompMainWin::updateButtonIcons()
{
	d->updateIcons(currentState_);
	d->updateShuffleIcon();
	d->updatePlaylistIcon();
	d->updateMuteIcon();
	d->updateIcons();
}


#include "qompmainwin.moc"
