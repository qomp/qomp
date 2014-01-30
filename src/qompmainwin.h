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
#include "qompplayer.h"

namespace Ui {
	class QompMainWin;
}

class QompPlayListModel;
class QModelIndex;
class QompTrayIcon;

class QompMainWin : public QMainWindow
{
	Q_OBJECT
	
public:
	QompMainWin(QWidget *parent = 0);
	~QompMainWin();

	QompPlayer* player() const;
	void setPlayer(QompPlayer* player);

	void bringToFront();

public slots:
	void actPlayActivated();
	void actPrevActivated();
	void actNextActivated();
	void actStopActivated();

	void toggleVisibility();
	void checkForUpdates();
	void aboutQomp();

private slots:
	void actOpenActivated();
	void actClearActivated();
	void muteButtonActivated(bool);
	void volumeSliderMoved(int);
	void volumeChanged(qreal vol);
	void seekSliderMoved(int);

	void mediaActivated(const QModelIndex& index);
	void mediaClicked(const QModelIndex& index);
	void doTrackContextMenu(const QPoint& p);

	void doMainContextMenu();

	void playerStateChanged(QompPlayer::State state);
	void setCurrentPosition(qint64 ms);
	void currentTotalTimeChanged(qint64 ms);
	void playNext();
	void doOptions();
	void updateTuneInfo();

	void loadPlaylist();
	void savePlaylist();

	//void trayDoubleclicked();
	void trayActivated(Qt::MouseButton);
	void trayWheeled(int delta);

	void updateIcons();
	void updateOptions();

	void tunes(const TuneList &list);
signals:
	void exit();

protected:
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);

private:
	void stopPlayer();
	
private:
	Ui::QompMainWin *ui;
	QompPlayer* player_;
	QompPlayListModel* model_;
	QompTrayIcon* trayIcon_;

	enum MainWinState { Stopped, Playing, Paused };
	MainWinState currentState_;
};

#endif // QOMPMAINWIN_H
