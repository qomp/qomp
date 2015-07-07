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

#ifndef QOMP_H
#define QOMP_H

#include <QModelIndex>

#include "common.h"

class QompMainWin;
class QompPlayListModel;
class QompPlayer;
class Tune;


class QompCon : public QObject
{
	Q_OBJECT
public:
	explicit QompCon(QObject *parent = 0);
	~QompCon();

public slots:
#ifdef Q_OS_ANDROID
	void incomingCall(bool begining);
#endif

private slots:
	void updateSettings();

	void actPlayNext();
	void actPlayPrev();
	void actPlay();
	void actPause();
	void actStop();

	void actMediaActivated(const QModelIndex& index);
	void actMediaClicked(const QModelIndex& index);

	void actMuteToggle(bool mute);
	void actSeek(int ms);
	void actSetVolume(qreal vol);

	void setTunes(const QList<Tune*>& tunes);

	void actClearPlaylist();
	void actRemoveSelected(const QModelIndexList& list);

	void actDoSettings();
	void actCheckForUpdates();
	void actAboutQomp();
	void actBugReport();

	void actDownloadTune(Tune* tune, const QString& dir);
	void actToggleTuneState(Tune* tune);
	void actRemoveTune(Tune* tune);
	void actCopyUrl(Tune* tune);
	void tuneUrlFinished(const QUrl& url);

	void mediaFinished(bool afterError = false);
	void playerStateChanged(Qomp::State state);
	void currentTuneChanged(Tune* t);

	void init();
	void deInit();
#ifdef HAVE_QT5
	void applicationStateChanged(Qt::ApplicationState state);
#endif

//protected:
//	bool eventFilter(QObject *obj, QEvent *e);

private:
	void setupMainWin();
	void connectMainWin();
	void disconnectMainWin();
	void setupPlayer();
	void setupModel();
	void checkVersion();

	void stopPlayer();
	void playIndex(const QModelIndex& index);

	static QompPlayer* createPlayer();

private:
	QompMainWin* mainWin_;
	QompPlayListModel* model_;
	QompPlayer* player_;
};

#endif // QOMP_H
