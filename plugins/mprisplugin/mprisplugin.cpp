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

#include "mprisplugin.h"
#include "qompplayer.h"
#include "tune.h"
#include "common.h"

#include <QTimer>
#include <QtPlugin>
#include <QApplication>
#include <QMainWindow>
#include <QToolButton>
#include <QStandardPaths>
#include <QTemporaryFile>
#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif
#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

#define STOPPED "Stopped"
#define PAUSED "Paused"
#define PLAYING "Playing"
#define nextButtonName "tb_next"
#define prevButtonName "tb_prev"

static const QSize maxArtSize(510,510);

MprisPlugin::MprisPlugin() :
	player_(nullptr),
	enabled_(true),
	mpris_(nullptr),
	tune_(nullptr),
	lastTune_(nullptr),
	artFile_(nullptr)
{
}

void MprisPlugin::qompPlayerChanged(QompPlayer *player)
{
	if(player_ != player) {
		if(player_) {
			disconnect(player_, &QompPlayer::stateChanged, this, &MprisPlugin::playerStatusChanged);
			disconnect(player_, &QompPlayer::tuneDataUpdated, this, &MprisPlugin::tuneUpdated);
		}

		player_ = player;
		if(player_) {
			connect(player_, &QompPlayer::stateChanged, this, &MprisPlugin::playerStatusChanged);
			//needed to update albumArt for online files
			connect(player_, &QompPlayer::tuneDataUpdated, this, &MprisPlugin::tuneUpdated);
		}
	}
}

void MprisPlugin::playerControlChanged(QompPlayerControl *control)
{
	Q_UNUSED(control)
}

void MprisPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
	if(enabled_) {
		mpris_ = new MprisController(this);
		tune_ = new QompMetaData();
		artFile_ = new QTemporaryFile(this);
		artFile_->setAutoRemove(true);
		connect(mpris_, &MprisController::play, this, &MprisPlugin::play);
		connect(mpris_, &MprisController::pause, this, &MprisPlugin::pause);
		connect(mpris_, &MprisController::stop, this, &MprisPlugin::stop);
		connect(mpris_, &MprisController::next, this, &MprisPlugin::next);
		connect(mpris_, &MprisController::previous, this, &MprisPlugin::previous);
		connect(mpris_, &MprisController::volumeChanged, this, &MprisPlugin::setVolume);
		connect(mpris_, &MprisController::sendQuit, this, &MprisPlugin::doQuit);
		connect(mpris_, &MprisController::sendRaise, this, &MprisPlugin::doRaise);
		connect(mpris_, &MprisController::updateVolume, this, &MprisPlugin::updateVolume);
		connect(mpris_, &MprisController::updatePosition, this, &MprisPlugin::updatePosition);
		connect(mpris_, &MprisController::positionChanged, this, &MprisPlugin::setPosition);
	}
	else {
		disconnect(mpris_);
		disableMpris();
		delete artFile_;
		artFile_ = 0;
	}
}

void MprisPlugin::play()
{
	if(player_) {
		player_->play();
	}
}

void MprisPlugin::pause()
{
	if(player_) {
		player_->pause();
	}
}

void MprisPlugin::stop()
{
	if(player_) {
		player_->stop();
	}
}

void MprisPlugin::next()
{
	if(player_) {
		QMainWindow *mainWin = Qomp::getMainWindow();
		if (mainWin) {
			QToolButton *nextBtn = mainWin->findChild<QToolButton *>(nextButtonName);
			if(nextBtn) {
				emit nextBtn->clicked();
			}
		}
	}
}

void MprisPlugin::previous()
{
	if(player_) {
		QMainWindow *mainWin = Qomp::getMainWindow();
		if (mainWin) {
			QToolButton *prevBtn = mainWin->findChild<QToolButton *>(prevButtonName);
			if(prevBtn) {
				emit prevBtn->clicked();
			}
		}
	}
}

void MprisPlugin::setVolume(const qreal &volume)
{
	if(player_) {
		player_->setVolume(volume);
		emit player_->volumeChanged(volume); //Temporary hack, FIXME
	}
}

void MprisPlugin::doQuit()
{
	qApp->quit();
}

void MprisPlugin::doRaise()
{
	QMainWindow *mainWin = Qomp::getMainWindow();
	if(mainWin) {
		if(mainWin->isHidden()) {
			mainWin->show();
		}
	}
}

void MprisPlugin::updateVolume()
{
	if(player_) {
		mpris_->setVolume(player_->volume());
	}
}

void MprisPlugin::updatePosition()
{
	if(player_) {
		mpris_->setPosition(player_->position()*1000); //microseconds
	}
}

void MprisPlugin::setPosition(const qreal &position)
{
	if(player_) {
		player_->setPosition(position); //miliseconds
	}
}

void MprisPlugin::unload()
{
	disableMpris();
}

void MprisPlugin::playerStatusChanged(Qomp::State state)
{
	if(!enabled_ || !mpris_ || !player_)
		return;

	switch(state) {
		case Qomp::StatePlaying:
			getMetaData(player_->currentTune());
			sendMetadata(PLAYING);
			break;
		case Qomp::StateStopped:
			sendMetadata(STOPPED);
			break;
		case Qomp::StatePaused:
			sendMetadata(PAUSED);
			break;
		default:
			break;
	}
}

void MprisPlugin::getMetaData(Tune *tune)
{
	if (tune && lastTune_ != tune) {
		lastTune_ = tune;
		const int num = tune->trackNumber.isEmpty() ? 0 : tune->trackNumber.toInt();
		tune_->artist = tune->artist;
		tune_->title= tune->title;
		tune_->album = tune->album;
		tune_->trackNumber = num;
		tune_->trackLength = Qomp::durationStringToSeconds(tune->duration)*1e6; //in microseconds
		if (!tune->file.isEmpty()) {
			tune_->url = (tune->file.startsWith("file://")) ? tune->file : "file://" + tune->file;
		}
		else {
			tune_->url = QString();
		}
		tune_->cover = getAlbumArtFile(tune->cover());
	}
}

QString MprisPlugin::getAlbumArtFile(const QImage &art)
{
	if(!art.isNull()) {
		if(artFile_->exists()) {
			artFile_->remove();
		}
		const QString tmpPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
		if(!tmpPath.isEmpty()) {
			QImage scaledArt = art;
			if((scaledArt.size().width() > maxArtSize.width())
			   || (scaledArt.size().height() > maxArtSize.height())) {
				scaledArt = scaledArt.scaled(maxArtSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
			artFile_->setFileName(tmpPath + "/qomp_" + QString::number(qrand()) + "_cover.png");
#else
			artFile_->setFileName(tmpPath + "/qomp_" + QString::number(QRandomGenerator::global()->generate()) + "_cover.png");
#endif
			if (artFile_->open()) {
				if (!scaledArt.save(artFile_, "PNG")) {
					artFile_->close();
					return QString();
				}
				artFile_->close();
				return artFile_->fileName();
			}
		}
	}
	return QString();
}

void MprisPlugin::tuneUpdated(Tune *tune)
{
	if(player_->state() == Qomp::StatePlaying) {
		getMetaData(tune);
		sendMetadata(PLAYING);
	}
}

void MprisPlugin::sendMetadata(const QString &status)
{
	if (status == STOPPED || status == PAUSED) {
		mpris_->sendData(status, QompMetaData());
	}
	else if (status == PLAYING){
		mpris_->sendData(status, *tune_);
	}
}

void MprisPlugin::disableMpris()
{
	delete mpris_;
	mpris_ = 0;
	delete tune_;
	tune_ = 0;
	lastTune_ = 0;
}
