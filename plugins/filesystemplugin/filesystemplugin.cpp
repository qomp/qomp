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

#include "filesystemplugin.h"
#include "options.h"
#include "tune.h"
#include "qomppluginaction.h"

#include <QFileDialog>
#include <QtPlugin>
#include <QMenu>


FilesystemPlugin::FilesystemPlugin()
{
}

QList<Tune*> FilesystemPlugin::getTunes()
{
	QFileDialog f(0, tr("Select file(s)"),
		      Options::instance()->getOption("filesystemplugin.lastdir", QDir::homePath()).toString(),
		      tr("Audio files(*.mp3 *.ogg *.wav *.flac);;All files(*)"));
	f.setFileMode(QFileDialog::ExistingFiles);
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptOpen);
#if defined HAVE_QT5 && defined Q_OS_ANDROID
	f.setWindowState(Qt::WindowMaximized);
#endif
	QList<Tune*> list;

	if(f.exec() == QFileDialog::Accepted) {
		QStringList files = f.selectedFiles();

		if(!files.isEmpty()) {
			QFileInfo fi (files.first());
			Options::instance()->setOption("filesystemplugin.lastdir", fi.dir().path());
		}

		foreach(const QString& file, files) {
			Tune* tune = new Tune(false);
			tune->file = file;
			list.append(tune);
		}
	}
	return list;
}

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
			Tune* tune = new Tune(false);
			tune->file = fi.absoluteFilePath();
			list.append(tune);
		}
	}

	return list;
}

QList<Tune *> FilesystemPlugin::getFolders()
{
	QFileDialog f(0, tr("Select folder(s)"),
			Options::instance()->getOption("filesystemplugin.lastdir",QDir::homePath()).toString()
		      );
	f.setFileMode(QFileDialog::Directory);
	f.setOption(QFileDialog::ShowDirsOnly, false);
	f.setViewMode(QFileDialog::List);
	f.setAcceptMode(QFileDialog::AcceptOpen);
#if defined HAVE_QT5 && defined Q_OS_ANDROID
	f.setWindowState(Qt::WindowMaximized);
#endif
	QList<Tune*> list;

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
	return list;
}

QompOptionsPage *FilesystemPlugin::options()
{
	return 0;
}

QList<QompPluginAction *> FilesystemPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;

	//Menu will be deleted at parent menu's destructor
	QMenu *m = new QMenu;

	QompPluginAction *act = new QompPluginAction(QIcon(), tr("File System"), 0, "", this);
	act->setMenu(m);
	l.append(act);

	act = new QompPluginAction(QIcon(), tr("Select Files"), this, "getTunes", this);
	m->addAction(act);

	act = new QompPluginAction(QIcon(), tr("Select Folders"), this, "getFolders", this);
	m->addAction(act);

	return l;
}


#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(filesystemplugin, FilesystemPlugin)
#endif
