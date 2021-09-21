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

#ifndef FILESYSTEMHELPER_H
#define FILESYSTEMHELPER_H

#include <QObject>
#include <QMap>

class FilesystemHelper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QStringList drivePathes READ drivePathes NOTIFY drivePathesChanged)
public:
	explicit FilesystemHelper(QObject *parent = nullptr);

	QStringList drivePathes() const;
	Q_INVOKABLE bool checkChildPath(const QString& root, const QString& path) const;
	Q_INVOKABLE QString convertPath2LocalUrl(const QString& path) const;
	Q_INVOKABLE QString driveForPath(const QUrl& path) const;
	Q_INVOKABLE QString pathForDrive(const QString& drive) const;

signals:
	void drivePathesChanged();

private:
	void loadPathes();

private:
	QMap<QString, QString> _pathes;
};

#endif // FILESYSTEMHELPER_H
