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

#ifndef QOMPMAINWIN_H
#define QOMPMAINWIN_H

#include <QMainWindow>
#include <QModelIndex>

#include "qompplayer.h"

namespace Ui {
	class QompMainWin;
}

class QompPlayListModel;
class QompTrayIcon;

class QompMainWin : public QMainWindow
{
	Q_OBJECT
	
public:
	QompMainWin(QWidget *parent = 0);
	~QompMainWin();

	QompPlayer* player() const;
	void setPlayer(QompPlayer* player);
	void setModel(QompPlayListModel* model);

	void bringToFront();

public slots:
	void actPlayActivated();
	void actPrevActivated();
	void actNextActivated();
	void actStopActivated();

	void toggleVisibility();

	void setMuteState(bool mute);
	void volumeChanged(qreal vol);
	void setCurrentPosition(qint64 ms);
	void currentTotalTimeChanged(qint64 ms);

	void playerStateChanged(QompPlayer::State state);

private slots:
	void actOpenActivated();
	void actClearActivated();

	void volumeSliderMoved(int);


	void mediaActivated(const QModelIndex& index);
	void mediaClicked(const QModelIndex& index);
	void doTrackContextMenu(const QPoint& p);

	void doMainContextMenu();

	void playNext();

	void updateTuneInfo(Tune *tune);

	//void trayDoubleclicked();
	void trayActivated(Qt::MouseButton);
	void trayWheeled(int delta);

	void updateIcons(QompPlayer::State state);
	void updateOptions();

	void tunes(const TuneList &list);

	void toggleTune(Tune* tune);
	void removeTune(Tune* tune);
	void removeSelectedIndexes();

signals:
	void exit();
	void downloadTune(Tune* tune);
	void loadPlaylist();
	void savePlaylist();
	void doOptions();
	void aboutQomp();
	void checkForUpdates();
	void actMuteActivated(bool);
	void volumeSliderMoved(qreal);
	void seekSliderMoved(int);

	void clearPlaylist();
	void removeSelected(const QModelIndexList&);

protected:
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);

private:
	void stopPlayer();

	void connectActions();
	void setIcons();
	void setupPlaylist();

	void saveWindowState();
	void restoreWindowState();
	
private:
	Ui::QompMainWin *ui;
	QompPlayer* player_;
	QompPlayListModel* model_;
	QompTrayIcon* trayIcon_;

	enum MainWinState { Stopped, Playing, Paused };
	MainWinState currentState_;
};

#endif // QOMPMAINWIN_H
