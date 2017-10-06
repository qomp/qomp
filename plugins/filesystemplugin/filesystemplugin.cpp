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


#ifdef QOMP_MOBILE
#include <QDir>
#include <QQuickItem>
#include <QEventLoop>
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

static QList<Tune*> getTunesRecursive(const QString& folder,
		QompPluginAction* action = nullptr, QThread* t = nullptr)
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
					QThread::msleep(sleepInterval);
					list.clear();
				}
			}
		}
	}

	return list;
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


//-----------------------------------//
//------------TunesThread------------//
//-----------------------------------//
class TunesThread : public QThread
{
	Q_OBJECT
public:
	TunesThread(const QStringList& files, QompPluginAction* act, QObject* p)
		: QThread(p),
		_files(files),
		_action(act)
	{
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
					QThread::msleep(sleepInterval);
					list.clear();
				}
			}
		}
		if(!isInterruptionRequested())
			_action->setTunesReady(list);
	}

private:
	QStringList _files;
	QompPluginAction* _action;
};



//-----------------------------------//
//----FilesystemPlugin::Private------//
//-----------------------------------//
class FilesystemPlugin::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QObject* p = 0) : QObject(p)
		,_tThread(nullptr)
#ifdef QOMP_MOBILE
		,loop_(new QEventLoop(this))
		,item_(0)
#endif
	{
	}

	~Private()
	{
		stop();
	}

	void stop();

public slots:
	void getTunes(QompPluginAction *act);
	void getFolders(QompPluginAction* act);
#ifdef QOMP_MOBILE
	void exitLoop()
	{
		loop_->exit(-1);
	}

private:
	QEventLoop* loop_;
	QQuickItem* item_;
#endif

private:
	TunesThread* _tThread;
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
	QStringList files;
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
	QompQmlEngine::instance()->addItem(item_);
	connect(item_, SIGNAL(accepted()), loop_, SLOT(quit()));
	connect(item_, SIGNAL(rejected()), SLOT(exitLoop()));
	connect(item_, SIGNAL(destroyed()), SLOT(exitLoop()));

	QVariant varFiles;
	int res = loop_->exec();
	if(!res) {
		const QString folder = item_->property("folder").toUrl().toLocalFile();
		Options::instance()->setOption("filesystemplugin.lastdir", folder);
		varFiles = item_->property("files");

	}
	QompQmlEngine::instance()->removeItem();

	if(!varFiles.isNull()) {
		foreach(const QVariant& var, varFiles.value<QVariantMap>().keys()) {
			files.append(var.toUrl().toLocalFile());
		}
	}

#else
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
#endif
	if(!files.isEmpty()) {
		_tThread = new TunesThread(files, act, this);
		_tThread->start();
	}
}

void FilesystemPlugin::Private::getFolders(QompPluginAction* act)
{
	stop();
	QStringList files;
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
	QompQmlEngine::instance()->addItem(item_);
	connect(item_, SIGNAL(accepted()), loop_, SLOT(quit()));
	connect(item_, SIGNAL(rejected()), SLOT(exitLoop()));
	connect(item_, SIGNAL(destroyed()), SLOT(exitLoop()));

	int res = loop_->exec();
	if(!res) {
		files.append(item_->property("folder").toUrl().toLocalFile());
		Options::instance()->setOption("filesystemplugin.lastdir", files.first());
	}
	QompQmlEngine::instance()->removeItem();

#else
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
#endif
	if(!files.isEmpty()) {
		_tThread = new TunesThread(files, act, this);
		_tThread->start();
	}
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
			tunes->append(getTunesRecursive(file));
			return true;
		}
		else
			return processString(file, tunes);
	}
	return false;
}


#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(filesystemplugin, FilesystemPlugin)
#endif

#include "filesystemplugin.moc"
