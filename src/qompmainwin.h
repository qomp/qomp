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

#ifndef QOMPMAINWIN_H
#define QOMPMAINWIN_H

#include <QModelIndex>

#include "common.h"

class QompPlayListModel;
class Tune;


class QompMainWin : public QObject
{
	Q_OBJECT
	
public:
	QompMainWin(QObject *parent = 0);
	~QompMainWin();

	void setModel(QompPlayListModel* model);

public slots:
	void toggleVisibility();
	void setMuteState(bool mute);
	void volumeChanged(qreal vol);
	void setCurrentPosition(qint64 ms);
	void currentTotalTimeChanged(qint64 ms);
	void playerStateChanged(Qomp::State state);
	void hide();
	void show();
	void loadPlaylist(const QString& fileName);
	void savePlaylist(const QString& fileName);

signals:
	void downloadTune(Tune* tune, const QString& dir);
	void doOptions();
	void aboutQomp();
	void bugReport();
	void checkForUpdates();
	void actMuteActivated(bool);
	void volumeSliderMoved(qreal);
	void seekSliderMoved(int);

	void clearPlaylist();
	void removeSelected(const QModelIndexList&);
	void toggleTuneState(Tune* tune);
	void removeTune(Tune* tune);
	void tunes(const QList<Tune*> &list);
	void copyUrl(Tune* tune);

	void actPlayActivated();
	void actPrevActivated();
	void actNextActivated();
	void actStopActivated();
	void mediaActivated(const QModelIndex& index);
	void mediaClicked(const QModelIndex& index);

protected:
	bool eventFilter(QObject *o, QEvent *e);
	
private:
	class Private;
	Private* d;
	friend class Privare;

	QompPlayListModel* model_;
	Qomp::State currentState_;
};

#endif // QOMPMAINWIN_H
