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

#include "filesystemplugin.h"
#include "options.h"
#include "tune.h"
#include "qomppluginaction.h"
#include "cueparser.h"
#include "filesystemcommon.h"


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
#include <QMediaPlaylist>

#include <QtPlugin>

static const QString CUE_SUFFIX = "cue";
static const QString M3U_SUFFIX = "m3u";

static QList<Tune*> getTunesRecursive(const QString& folder)
{
	QList<Tune*> list;

	QDir dir(folder);
	foreach(const QString& entry, dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
		QFileInfo fi(dir.absolutePath() + "/" +entry);
		if(fi.isDir()) {
			list.append(getTunesRecursive(fi.absoluteFilePath()));
		}
		else  {
			static const QRegExp songRe("\\.(mp3|ogg|flac|wav|mp4)$");
			QString song = fi.absoluteFilePath();
			if(songRe.indexIn(song) != -1)
				list.append(Qomp::tuneFromFile(song));
		}
	}

	return list;
}

static QList<Tune*> m3uToTunes(const QString& file)
{
	QList<Tune*> list;

	if(QFile::exists(file)) {
		QMediaPlaylist p;
		p.load(QUrl::fromLocalFile(file));
		if (!p.isEmpty()) {
			for(int i = 0; i < p.mediaCount(); ++i) {
				list.append(Qomp::tuneFromFile(p.media(i).canonicalUrl().toLocalFile()));
			}
		}
	}

	return list;
}


class FilesystemPlugin::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QObject* p = 0) : QObject(p)
#ifdef QOMP_MOBILE
		,loop_(new QEventLoop(this))
		,item_(0)
#endif
	{
	}

public slots:
	QList<Tune*> getTunes();
	QList<Tune*> getFolders();
#ifdef QOMP_MOBILE
	void exitLoop()
	{
		loop_->exit(-1);
	}

private:
	QEventLoop* loop_;
	QQuickItem* item_;
#endif
};

QList<Tune*> FilesystemPlugin::Private::getTunes()
{
	QList<Tune*> list;
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
				<< tr("Playlists (*.m3u)")
				<< tr("All files (*.*)") );
	QompQmlEngine::instance()->addItem(item_);
	connect(item_, SIGNAL(accepted()), loop_, SLOT(quit()));
	connect(item_, SIGNAL(rejected()), SLOT(exitLoop()));
	connect(item_, SIGNAL(destroyed()), SLOT(exitLoop()));

	int res = loop_->exec();
	if(!res) {
		const QString folder = item_->property("folder").toUrl().toLocalFile();
		Options::instance()->setOption("filesystemplugin.lastdir", folder);
		QVariant varFiles = item_->property("files");
		foreach(const QVariant& var, varFiles.value<QVariantMap>().keys()) {
			const QString str = var.toUrl().toLocalFile();
			if(!str.isEmpty()) {
				QFileInfo fi(str);
				if(fi.suffix() == CUE_SUFFIX) {
					list.append(CueParser::parseTunes(str));
				}
				else if(fi.suffix() == M3U_SUFFIX) {
					list.append(m3uToTunes(str));
				}
				else {
					list.append(Qomp::tuneFromFile(str));
				}
			}
		}
	}
	QompQmlEngine::instance()->removeItem();
#else
	QFileDialog f(0, tr("Select file(s)"),
		      Options::instance()->getOption("filesystemplugin.lastdir", QDir::homePath()).toString(),
		      tr("Audio files(*.mp3 *.ogg *.wav *.flac *.cue);;Playlists (*.m3u);;All files(*)"));
	f.setFileMode(QFileDialog::ExistingFiles);
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptOpen);

	if(f.exec() == QFileDialog::Accepted) {
		QStringList files = f.selectedFiles();

		if(!files.isEmpty()) {
			QFileInfo fi (files.first());
			Options::instance()->setOption("filesystemplugin.lastdir", fi.dir().path());
		} 

		foreach(const QString& file, files) {
			QFileInfo fi(file);
			if(fi.suffix() == CUE_SUFFIX) {
				list.append(CueParser::parseTunes(file));
			}
			else if(fi.suffix() == M3U_SUFFIX) {
				list.append(m3uToTunes(file));
			}
			else {
				list.append(Qomp::tuneFromFile(file));
			}
		}
	}
#endif
	return list;
}

QList<Tune *> FilesystemPlugin::Private::getFolders()
{
	QList<Tune*> list;
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
		const QString folder = item_->property("folder").toUrl().toLocalFile();
		Options::instance()->setOption("filesystemplugin.lastdir", folder);
		list = getTunesRecursive(folder);
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
		QStringList files = f.selectedFiles();

		if(!files.isEmpty()) {
			QFileInfo fi (files.first());
			Options::instance()->setOption("filesystemplugin.lastdir", fi.dir().path());
		}

		foreach(const QString& file, files) {
			QFileInfo fi(file);
			if(fi.isDir()) {
				list.append(getTunesRecursive(file));
			}
		}
	}
#endif
	return list;
}




FilesystemPlugin::FilesystemPlugin() : QObject()
{
	d = new Private(this);
}

QompOptionsPage *FilesystemPlugin::options()
{
	return 0;
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
		if(fi.suffix() == CUE_SUFFIX) {
			tunes->append(CueParser::parseTunes(file));
		}
		else if(fi.suffix() == M3U_SUFFIX) {
			tunes->append(m3uToTunes(file));
		}
		else {
			tunes->append(Qomp::tuneFromFile(file));
		}
		return true;
	}
	else
		return false;
}


#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(filesystemplugin, FilesystemPlugin)
#endif

#include "filesystemplugin.moc"
