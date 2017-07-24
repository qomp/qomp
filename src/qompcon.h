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

#ifndef QOMP_H
#define QOMP_H

#include <QModelIndex>
#include <QPointer>

#include "common.h"
#include "qompinstancewatcher.h"
#include "qompplayercontrol.h"

class QompMainWin;
class QompPlayListModel;
class QompPlayer;
class Tune;
class QompCommandLine;
class QompOptionsDlg;


class QompCon : public QObject, public QompPlayerControl
{
	Q_OBJECT
	Q_INTERFACES(QompPlayerControl)
public:
	explicit QompCon(QObject *parent = 0);
	~QompCon();

public slots:
	void incomingCall(bool begining);

	//QompPlayerControl
	virtual void actPlayNext() Q_DECL_OVERRIDE;
	virtual void actPlayPrev() Q_DECL_OVERRIDE;
	virtual void actPlay() Q_DECL_OVERRIDE;
	virtual void actPause() Q_DECL_OVERRIDE;
	virtual void actStop() Q_DECL_OVERRIDE;
	virtual void actMuteToggle(bool mute) Q_DECL_OVERRIDE;
	virtual void actSeek(int ms) Q_DECL_OVERRIDE;
	virtual void actSetVolume(qreal vol) Q_DECL_OVERRIDE;

private slots:
	void updateSettings();

	void actMediaActivated(const QModelIndex& index);
	void actMediaClicked(const QModelIndex& index);

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
	void processUrl(const QString& url);
	void applicationStateChanged(Qt::ApplicationState state);

protected:
	bool eventFilter(QObject *obj, QEvent *e);

private:
	void setupMainWin();
	void connectMainWin();
	void disconnectMainWin();
	void setupPlayer();
	void setupModel();
	void checkVersion();
	void connectMediaKeys();

	void stopPlayer();
	void playIndex(const QModelIndex& index);
	void playNextShuffle(bool afterError, const QModelIndex& index);
	void playNext(bool afterError, const QModelIndex& index);

	void savePlayerPosition();
	void savePlayerPosition(qint64 pos);
	void preparePlayback();
	void processCommandLine();
	bool setupWatcher();
	void findNextMedia(bool afterError);

	static QompPlayer* createPlayer();

private:
	QompMainWin* mainWin_;
	QompPlayListModel* model_;
	QompPlayer* player_;
	QompCommandLine* commandLine_;
	QPointer<QompOptionsDlg> optionsDlg_;
#ifndef QOMP_MOBILE
	QompInstanceWatcher watcher_;
#endif
};

#endif // QOMP_H
