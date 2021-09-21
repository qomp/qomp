/*
 * Copyright (C) 2021  Khryukin Evgeny
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

#include "filesystemhelper.h"

#include <QAndroidJniObject>
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QFileInfo>
#include <QUrl>

FilesystemHelper::FilesystemHelper(QObject *parent) : QObject(parent)
{
	loadPathes();
}

QStringList FilesystemHelper::drivePathes() const
{
	return _pathes;
}

bool FilesystemHelper::checkChildPath(const QString &root, const QString &path) const
{
	QFileInfo ri(root);
	QFileInfo p(path);
	return p.absoluteFilePath().startsWith(ri.absoluteFilePath());
}

QString FilesystemHelper::convertPath2LocalUrl(const QString &path) const
{
	return QUrl::fromLocalFile(path).toString();
}

void FilesystemHelper::loadPathes()
{
	_pathes.clear();

	const QAndroidJniObject pathes = QtAndroid::androidActivity().callObjectMethod("drivePathes", "()[Ljava/lang/String;");
	QAndroidJniEnvironment jni;
	const jsize len = jni->GetArrayLength(pathes.object<jarray>());
	for(int i = 0; i < len; ++i) {
		const jobject path = jni->GetObjectArrayElement(pathes.object<jobjectArray>(), i);
		_pathes.append(QAndroidJniObject(path).toString());
	}
}
