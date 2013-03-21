#include "qompmainwin.h"
#include "qompplayer.h"
#include "playlistmodel.h"
#include "options.h"
#include "tune.h"
#include "pluginmanager.h"

#include "ui_qompmainwin.h"

#include <QTime>
#include <QNetworkAccessManager>


QompMainWin::QompMainWin(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::QompMainWin),
	nam_(new QNetworkAccessManager(this))
{
	ui->setupUi(this);

	player_ = new QompPlayer(this);
	player_->setSeekSlider(ui->seekSlider);
	player_->setVolumeSlider(ui->volumeSlider);

	connect(player_, SIGNAL(currentPosition(qint64)), SLOT(setCurrentPosition(qint64)));

	model_ = new PlayListModel(this);
	ui->playList->setModel(model_);

	ui->tb_next->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
	ui->tb_prev->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
	ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	ui->tb_stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
	ui->tb_clear->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
	ui->tb_open->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));

	setCurrentPosition(0);

	connect(ui->tb_open, SIGNAL(clicked()), SLOT(actOpenActivated()));
	connect(ui->tb_play, SIGNAL(clicked()), SLOT(actPlayActivated()));
	connect(ui->tb_stop, SIGNAL(clicked()), SLOT(actStopActivated()));
	connect(ui->tb_clear, SIGNAL(clicked()), SLOT(actClearActivated()));
	connect(ui->tb_next, SIGNAL(clicked()), SLOT(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()), SLOT(actPrevActivated()));

	connect(ui->actionOpen, SIGNAL(triggered()), SLOT(actOpenActivated()));
	connect(ui->actionExit, SIGNAL(triggered()), SLOT(close()));

	connect(ui->playList, SIGNAL(activated(QModelIndex)), SLOT(mediaActivated(QModelIndex)));
	connect(ui->playList, SIGNAL(clicked(QModelIndex)), SLOT(mediaClicked(QModelIndex)));
	connect(player_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(updatePlayIcon()));
	connect(player_, SIGNAL(mediaFinished()), SLOT(playNext()));
}


QompMainWin::~QompMainWin()
{
	delete ui;
}


void QompMainWin::actPlayActivated()
{
	QModelIndex i = model_->indexForTune(model_->currentTune());
	if(!i.isValid())
		return;

	if(!(player_->currentSource() == model_->device(i)))
		player_->setSource(model_->device(i));

	player_->play();
	ui->lb_artist->setText(i.data(PlayListModel::ArtistRole).toString());
	ui->lb_title->setText(i.data(PlayListModel::TitleRole).toString());
}

void QompMainWin::updatePlayIcon()
{
	if(player_->state() == Phonon::PlayingState)
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
	else
		ui->tb_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}

void QompMainWin::actPrevActivated()
{
	QModelIndex index = ui->playList->currentIndex();
	if(index.isValid() && index.row() > 0) {
		bool play = (player_->state() == Phonon::PlayingState);
		index = model_->index(index.row()-1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		if(play)
			actPlayActivated();
	}
}

void QompMainWin::actNextActivated()
{
	QModelIndex index = ui->playList->currentIndex();
	if(index.isValid() && index.row() < model_->rowCount()-1) {
		bool play = (player_->state() == Phonon::PlayingState);
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
	QModelIndex index = ui->playList->currentIndex();
	if(index.isValid())
		model_->setCurrentTune(model_->tune(index));
	ui->lb_artist->setText("");
	ui->lb_title->setText("");
}

void QompMainWin::actOpenActivated()
{
	TuneList list;
	QMenu m;
	QList<QAction*> acts;
	foreach(const QString& name, PluginManager::instance()->availablePlugins()) {
		acts.append(new QAction(name, &m));
	}
	m.addActions(acts);
	m.move(QCursor::pos());
	QAction* x = m.exec();
	if(x)
		list = PluginManager::instance()->getTune(x->text());


	if(!list.isEmpty()) {
		model_->addTunes(list);
		if(player_->state() == Phonon::StoppedState) {
			QModelIndex index = model_->indexForTune(list.first());
			ui->playList->setCurrentIndex(index);
			model_->setCurrentTune(model_->tune(index));
			actPlayActivated();
		}
	}
}

void QompMainWin::actClearActivated()
{
	player_->stop();
	model_->clear();
}

void QompMainWin::mediaActivated(const QModelIndex &index)
{
	model_->setCurrentTune(model_->tune(index));
	actPlayActivated();
}

void QompMainWin::mediaClicked(const QModelIndex &index)
{
	if(player_->state() == Phonon::StoppedState) {
		model_->setCurrentTune(model_->tune(index));
	}
}

void QompMainWin::setCurrentPosition(qint64 ms)
{
	int h = ms / (1000*60*60);
	ms -= h*(1000*60*60);
	int m = ms / (1000*60);
	ms -= m*(1000*60);
	int s = ms / 1000;
	ms -= s*1000;
	QTime t(h, m, s, ms);
	const QString format = h ? "hh:mm:ss" : "mm:ss";
	ui->lcd->display(t.toString(format));
}

void QompMainWin::playNext()
{
	if(model_->indexForTune(model_->currentTune()).row() == model_->rowCount()-1) {
		actStopActivated();
	}
	else {
		QModelIndex index = model_->indexForTune(model_->currentTune());
		index = model_->index(index.row()+1);
		ui->playList->setCurrentIndex(index);
		model_->setCurrentTune(model_->tune(index));
		actPlayActivated();
	}
}
