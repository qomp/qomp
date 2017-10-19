/*
 * Copyright (C) 2013-2017  Khryukin Evgeny
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

#include "filesystemplugin.h"
#include "options.h"
#include "tune.h"
#include "qomppluginaction.h"
#include "cueparser.h"
#include "defines.h"
#include "taghelpers.h"
#include "playlistparser.h"
#include "qompplayer.h"


#ifdef QOMP_MOBILE

static const char* FOLDER_PROP = "get_folder";

#include <QDir>
#include <QQuickItem>
#include "qompqmlengine.h"
#include "common.h"
#else
#include <QFileDialog>
#include <QMenu>
#endif
#include <QMimeDatabase>
#include <QTextStream>
#include <QThread>

#include <QtPlugin>

static const QString CUE_TYPE = "application/x-cue";
static const QString supportedMimeTypesPrefix = "audio/";
static const int maxTunesForList = 10;
static const int sleepInterval = 10; //msec


//-----------------------------------//
//---------PlayerContainer-----------//
//-----------------------------------//
class PlayerContainer
{
public:
	void setPlayer(QompPlayer* p)
	{
		_player = p;
	}

	QompPlayer* player() const
	{
		return _player;
	}

private:
	QompPlayer* _player;
};


//-----------------------------------//
//------------TunesThread------------//
//-----------------------------------//
class TunesThread : public QThread
{
	Q_OBJECT
public:
	TunesThread(const QStringList& files, QompPluginAction* act, PlayerContainer* p)
		: QThread(),
		_files(files),
		_action(act),
		_pc(p)
	{
	}

	void waitForPLayer()
	{
		while(!isInterruptionRequested()
			&& qp()->state() != Qomp::StatePlaying
			&& qp()->state() != Qomp::StatePaused )
		{
			QThread::msleep(sleepInterval);
		}
	}

	static bool processString(const QString& file, QList<Tune*> *tunes)
	{
		if(!file.isEmpty()) {
			PlaylistParser p(file);
			if(p.canParse()) {
				tunes->append(p.parse());
				return true;
			}
			else {
				const QString mimeType = QMimeDatabase().mimeTypeForFile(file).name();
				if(mimeType.compare(CUE_TYPE, Qt::CaseInsensitive) == 0) {
					tunes->append(CueParser::parseTunes(file));
					return true;
				}
				else if (mimeType.startsWith(supportedMimeTypesPrefix, Qt::CaseInsensitive)) {
					tunes->append(Qomp::tuneFromFile(file));
					return true;
				}
			}
		}
		return false;
	}

	static QList<Tune*> getTunesRecursive(const QString& folder,
			QompPluginAction* action = nullptr, TunesThread* t = nullptr)
	{
		QList<Tune*> list;

		QDir dir(folder);
		foreach(const QString& entry, dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
			if(t && t->isInterruptionRequested())
				break;

			QFileInfo fi(dir.absolutePath() + "/" +entry);
			if(fi.isDir()) {
				list.append(getTunesRecursive(fi.absoluteFilePath(), action, t));
			}
			else  {
				const QString mimeType = QMimeDatabase().mimeTypeForFile(fi).name();
				if (mimeType.startsWith(supportedMimeTypesPrefix, Qt::CaseInsensitive)) {
					list.append(Qomp::tuneFromFile(fi.absoluteFilePath()));
					if(action && list.count() >= maxTunesForList) {
						action->setTunesReady(list);
						if(t)
							t->waitForPLayer();
						list.clear();
					}
				}
			}
		}

		return list;
	}

protected:
	virtual void run() Q_DECL_OVERRIDE
	{
		QList<Tune*> list;
		foreach(const QString& file, _files) {
			if(isInterruptionRequested())
				break;
			QFileInfo fi(file);
			if(fi.isDir()) {
				list.append(getTunesRecursive(file, _action, this));
			}
			else {
				processString(file, &list);
				if(list.count() >= maxTunesForList) {
					_action->setTunesReady(list);
					waitForPLayer();
					list.clear();
				}
			}
		}
		if(!isInterruptionRequested())
			_action->setTunesReady(list);
	}

private:
	QompPlayer* qp() const
	{
		return _pc->player();
	}

private:
	QStringList _files;
	QompPluginAction* _action;
	PlayerContainer* _pc;
};



//-----------------------------------//
//----FilesystemPlugin::Private------//
//-----------------------------------//
class FilesystemPlugin::Private : public QObject, public PlayerContainer
{
	Q_OBJECT
public:
	explicit Private(QObject* p = 0) : QObject(p)
#ifdef QOMP_MOBILE
		,item_(0)
#endif
		,_tThread(nullptr)
	{
		setPlayer(nullptr);
	}

	~Private()
	{
		stop();
	}

	void stop();

	void processFiles(const QStringList& files)
	{
		if(!files.isEmpty()) {
			_tThread = new TunesThread(files, _act, this);
			_tThread->start();
		}
	}

public slots:
	void getTunes(QompPluginAction *act);
	void getFolders(QompPluginAction* act);
#ifdef QOMP_MOBILE
	void accepted()
	{
		QStringList files;

		const QString folder = item_->property("folder").toUrl().toLocalFile();
		Options::instance()->setOption("filesystemplugin.lastdir", folder);

		if(item_->property(FOLDER_PROP).toBool()) {
			files.append(folder);
		}
		else {
			const QVariant varFiles = item_->property("files");

			if(!varFiles.isNull()) {
				foreach(const QVariant& var, varFiles.value<QVariantMap>().keys()) {
					files.append(var.toUrl().toLocalFile());
				}
			}
		}

		processFiles(files);
		QompQmlEngine::instance()->removeItem();
	}

	void rejected()
	{
		QompQmlEngine::instance()->removeItem();
	}
private:
	QQuickItem* item_;
#endif

private:
	TunesThread* _tThread;
	QompPluginAction* _act;
};

void FilesystemPlugin::Private::stop()
{
	if(_tThread &&_tThread->isRunning()) {
		_tThread->requestInterruption();
		_tThread->wait();
	}

	delete _tThread;
	_tThread = nullptr;
}

void FilesystemPlugin::Private::getTunes(QompPluginAction* act)
{
	stop();
	_act = act;
#ifdef QOMP_MOBILE
	item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qmlshared/QompFileDlg.qml"));
	item_->setProperty("selectFolders", false);
	item_->setProperty("singleSelect", false);
	item_->setProperty("selectExisting", true);
	item_->setProperty("title", tr("Select file(s)"));
	item_->setProperty("folder", QUrl::fromLocalFile(
				   Qomp::safeDir(Options::instance()->getOption("filesystemplugin.lastdir").toString())
						));
	item_->setProperty("filter", QStringList()
				<< tr("Audio files (*.mp3, *.ogg, *.wav, *.flac, *.cue)")
				<< tr("Playlists (*.m3u, *.m3u8, *.pls)")
				<< tr("All files (*.*)") );
	item_->setProperty(FOLDER_PROP, false);

	connect(item_, SIGNAL(accepted()), SLOT(accepted()));
	connect(item_, SIGNAL(rejected()), SLOT(rejected()));

	QompQmlEngine::instance()->addItem(item_);
#else
	QStringList files;
	QFileDialog f(0, tr("Select file(s)"),
		      Options::instance()->getOption("filesystemplugin.lastdir", QDir::homePath()).toString(),
		      tr("Audio files (*.mp3 *.ogg *.wav *.flac *.cue);;Playlists (*.m3u *.m3u8 *.pls);;All files (*)"));
	f.setFileMode(QFileDialog::ExistingFiles);
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptOpen);

	if(f.exec() == QFileDialog::Accepted) {
		files = f.selectedFiles();

		if(!files.isEmpty()) {
			QFileInfo fi (files.first());
			Options::instance()->setOption("filesystemplugin.lastdir", fi.dir().path());
		}
	}
	processFiles(files);
#endif
}

void FilesystemPlugin::Private::getFolders(QompPluginAction* act)
{
	stop();
	_act = act;
#ifdef QOMP_MOBILE
	item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qmlshared/QompFileDlg.qml"));
	item_->setProperty("selectFolders", true);
	item_->setProperty("singleSelect", true);
	item_->setProperty("selectExisting", true);
	item_->setProperty("title", tr("Select folder"));
	item_->setProperty("folder", QUrl::fromLocalFile(
				  Qomp::safeDir(Options::instance()->getOption("filesystemplugin.lastdir").toString())
						));
	item_->setProperty("filter", QStringList() << tr("All files (*.*)") );
	item_->setProperty(FOLDER_PROP, true);

	connect(item_, SIGNAL(accepted()), SLOT(accepted()));
	connect(item_, SIGNAL(rejected()), SLOT(rejected()));

	QompQmlEngine::instance()->addItem(item_);
#else
	QStringList files;
	QFileDialog f(0, tr("Select folder"),
			Options::instance()->getOption("filesystemplugin.lastdir",QDir::homePath()).toString()
		      );
	f.setFileMode(QFileDialog::Directory);
	f.setOption(QFileDialog::ShowDirsOnly, false);
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptOpen);

	if(f.exec() == QFileDialog::Accepted) {
		files = f.selectedFiles();

		if(!files.isEmpty()) {
			QFileInfo fi (files.first());
			Options::instance()->setOption("filesystemplugin.lastdir", fi.absoluteFilePath());
		}
	}
	processFiles(files);
#endif
}


//-----------------------------------//
//---------FilesystemPlugin----------//
//-----------------------------------//
FilesystemPlugin::FilesystemPlugin() : QObject()
{
	d = new Private(this);
}

QompOptionsPage *FilesystemPlugin::options()
{
	return 0;
}

void FilesystemPlugin::unload()
{
	d->stop();
}

QList<QompPluginAction *> FilesystemPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;

	QompPluginAction *act = 0;
	QObject* parentObj = this;
#ifndef QOMP_MOBILE
	//Menu will be deleted at parent menu's destructor
	QMenu *m = new QMenu;

	act = new QompPluginAction(QIcon(), tr("File System"), 0, "", parentObj);
	act->action()->setMenu(m);
	l.append(act);
	parentObj = act;
#endif

	act = new QompPluginAction(QIcon(), tr("Select Files"), d, "getTunes", parentObj);
#ifdef QOMP_MOBILE
	l.append(act);
#else
	m->addAction(act->action());
#endif

	act = new QompPluginAction(QIcon(), tr("Select Folders"), d, "getFolders", parentObj);
#ifdef QOMP_MOBILE
	l.append(act);
#else
	m->addAction(act->action());
#endif

	return l;
}

bool FilesystemPlugin::processUrl(const QString &url, QList<Tune *> *tunes)
{
	QFileInfo fi(url);
	if(!fi.exists()) {
		QUrl u(url);
		if(u.isValid() && u.isLocalFile()) {
			fi.setFile(u.toLocalFile());
		}
	}

	if(fi.exists()) {
		const QString file = fi.canonicalFilePath();
		if (fi.isDir()) {
			tunes->append(TunesThread::getTunesRecursive(file));
			return true;
		}
		else
			return TunesThread::processString(file, tunes);
	}
	return false;
}

void FilesystemPlugin::qompPlayerChanged(QompPlayer *player)
{
	d->setPlayer(player);
}


#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(filesystemplugin, FilesystemPlugin)
#endif

#include "filesystemplugin.moc"
