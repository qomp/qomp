/*
 * Copyright (C) 2016  Khryukin Evgeny
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

#ifndef CUEPARSER_H
#define CUEPARSER_H


#include <QString>

struct Cd;
class Tune;

class CueParser
{
public:
	explicit CueParser(const QString& fname);
	~CueParser();

	static QList<Tune*> parseTunes(const QString& fname);

	bool isValid() const;
	int filesCount() const;
	Tune* tune(int index) const;
	const QString& artist() const;
	const QString& album() const;

private:
	static qint64 framesLenToMiliSecs(long frames);

private:
	Cd* _cd;
	QString _path;
	mutable QString _artist;
	mutable QString _album;
};

#endif // CUEPARSER_H
