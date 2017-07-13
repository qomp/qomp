/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#ifndef PLAYLISTPARSER_H
#define PLAYLISTPARSER_H

#include <QObject>
#include <QUrl>
#include "libqomp_global.h"

class Tune;

class LIBQOMPSHARED_EXPORT PlaylistParser : public QObject
{
	Q_OBJECT
public:
	PlaylistParser(const QString& fileName, QObject *parent = nullptr);

	bool canParse() const;
	QList<Tune*> parse();

	static QList<Tune*> parseM3U(QByteArray* data, const QUrl &fileLocation = QUrl());
	static QList<Tune*> parsePLS(QByteArray* data);

private:
	QString _file;
	QString _mimeType;
};

#endif // PLAYLISTPARSER_H
