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

#include <QFileDialog>
#include <QtPlugin>

FilesystemPlugin::FilesystemPlugin()
{
}

TuneList FilesystemPlugin::getTunes()
{
	QStringList files = QFileDialog::getOpenFileNames(0, tr("Select file(s)"),
					Options::instance()->getOption("filesystemplugin.lastdir", QDir::homePath()).toString(),
					"*mp3 *ogg *wav *flac");

	if(!files.isEmpty()) {
		QFileInfo fi (files.first());
		Options::instance()->setOption("filesystemplugin.lastdir", fi.dir().path());
	}
	TuneList list;
	foreach(const QString& file, files) {
		Tune tune;
		tune.file = file;
		list.append(tune);
	}
	return list;
}

Q_EXPORT_PLUGIN2(filesystemplugin, FilesystemPlugin)
