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

#include "common.h"

namespace Ui {
	class QompMainWin;
}

class QompPlayListModel;
class QompTrayIcon;
class Tune;

class QompMainWin : public QMainWindow
{
	Q_OBJECT
	
public:
	QompMainWin(QWidget *parent = 0);
	~QompMainWin();

	void setModel(QompPlayListModel* model);
	void bringToFront();

public slots:
	void toggleVisibility();
	void setMuteState(bool mute);
	void volumeChanged(qreal vol);
	void setCurrentPosition(qint64 ms);
	void currentTotalTimeChanged(qint64 ms);
	void playerStateChanged(Qomp::State state);

private slots:
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
	void toggleTuneState(Tune* tune);
	void removeTune(Tune* tune);
	void tunes(const QList<Tune*> &list);

	void actPlayActivated();
	void actPrevActivated();
	void actNextActivated();
	void actStopActivated();
	void mediaActivated(const QModelIndex& index);
	void mediaClicked(const QModelIndex& index);

protected:
	void closeEvent(QCloseEvent *e);
	void changeEvent(QEvent *e);

private:
	void connectActions();
	void setIcons();
	void setupPlaylist();

	void saveWindowState();
	void restoreWindowState();
	
private:
	Ui::QompMainWin *ui;
	QompPlayListModel* model_;
	QompTrayIcon* trayIcon_;

	Qomp::State currentState_;
};

#endif // QOMPMAINWIN_H
