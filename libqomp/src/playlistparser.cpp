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

#include "playlistparser.h"
#include "tune.h"
#include "common.h"
#include "taghelpers.h"
#include "defines.h"

#include <QFile>
#include <QMimeDatabase>
#include <QTextStream>
#include <QBuffer>
#include <QTextCodec>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const QString PLS_TYPE = "audio/x-scpls";
static const QString M3U_TYPE = "audio/x-mpegurl";
static const QString PLAIN_TYPE = "text/plain";
static const QString EXTINF = "#EXTINF:";

PlaylistParser::PlaylistParser(const QString &fileName, QObject *parent) :
	QObject(parent),
	 _file(fileName)
{
	if(QFile::exists(_file)) {
		_mimeType = QMimeDatabase().mimeTypeForFile(_file).name();
#ifdef DEBUG_OUTPUT
		qDebug() << "PlaylistParser MIME type: " << _mimeType;
#endif
	}
}

bool PlaylistParser::canParse() const
{
	QStringList types{PLS_TYPE, M3U_TYPE};

	for(const QString& type: types) {
		if(_mimeType.compare(type, Qt::CaseInsensitive) == 0)
			return true;
	}

	if(checkQompPlaylist()) {
		return true;
	}

	return false;
}

QList<Tune *> PlaylistParser::parse()
{
	QList<Tune *> tunes;

	if(QFile::exists(_file)) {
		QFile f(_file);
		if (f.open(QFile::ReadOnly)) {
			QByteArray data = f.readAll();
			if(_mimeType.compare(M3U_TYPE) == 0) {
				tunes = parseM3U(&data, QUrl::fromLocalFile(_file));
			}
			else if(_mimeType.compare(PLS_TYPE) == 0) {
				tunes = parsePLS(&data);
			}
			else if(checkQompPlaylist()) {
				tunes = Tune::tunesFromFile(_file);
			}

		}
	}
	return tunes;
}

QList<Tune *> PlaylistParser::parseM3U(QByteArray *data, const QUrl &fileLocation)
{
	QList<Tune*> list;

	uint uc, ic;
	uc = ic = 0;
	auto createNeededTunesCount = [&list](int num) {
		++num;
		while(list.count() < num)
			list.append(new Tune);
	};


	QBuffer buf(data);
	buf.open(QBuffer::ReadOnly);
	QTextStream ts(&buf);

	if(Qomp::checkIsUTF(*data))
		ts.setCodec(QTextCodec::codecForName("UTF-8"));

	//Next piece of code partially get from qtmultimedia m3u plugin
	while (!ts.atEnd()) {
		QUrl nextResource;
		QString line = ts.readLine().trimmed();
		if (line.isEmpty() || (line[0] == '#' && !line.startsWith(EXTINF)) || line.size() > 4096)
			continue;

		if(line.startsWith(EXTINF)) {
			createNeededTunesCount(ic);
			if(list.at(ic)->file.isEmpty()) {
				const QString title = line.replace(EXTINF, "").replace(QRegExp("^\\d+,"), "");
				list.at(ic)->title = title;
			}
			++ic;

			continue;
		}

		QUrl fileUrl = QUrl::fromLocalFile(line);
		QUrl url(line);

		//m3u may contain url encoded entries or absolute/relative file names
		//prefer existing file if any
		QList<QUrl> candidates;
		if (!fileLocation.isEmpty()) {
			candidates << fileLocation.resolved(fileUrl);
			candidates << fileLocation.resolved(url);
		}
		candidates << fileUrl;
		candidates << url;

		for (const QUrl &candidate : candidates) {
			if (QFile::exists(candidate.toLocalFile())) {
				nextResource = candidate;
				break;
			}
		}

		if (nextResource.isEmpty()) {
			//assume the relative urls are file names, not encoded urls if m3u is local file
			if (!fileLocation.isEmpty() && url.isRelative()) {
				if (fileLocation.scheme() == QLatin1String("file"))
					nextResource = fileLocation.resolved(fileUrl);
				else
					nextResource = fileLocation.resolved(url);
			}
		}


		createNeededTunesCount(uc);
		if(!nextResource.isEmpty()) {
			list.at(uc)->deleteLater();
			list[uc] = Qomp::tuneFromFile(nextResource.toLocalFile());

		}
		else {
			list.at(uc)->url = url.toString();
		}
		++uc;
	}

	return list;
}

QList<Tune *> PlaylistParser::parsePLS(QByteArray *data)
{
	QList<Tune*> list;
	auto createNeededTunesCount = [&list](int num) {
		while(list.count() < num)
			list.append(new Tune);
	};

	QBuffer buf(data);
	buf.open(QBuffer::ReadOnly);
	QTextStream ts(&buf);

	if(Qomp::checkIsUTF(*data))
		ts.setCodec(QTextCodec::codecForName("UTF-8"));

	while (!ts.atEnd()) {
		QString line = ts.readLine().trimmed();
		if(!line.contains("="))
			continue;

		QStringList sl = line.split("=", QString::SkipEmptyParts);

		if (sl.first().startsWith("File")) {
			QRegExp re("File(\\d+)");
			if(re.indexIn(sl.first()) != -1) {
				int num = re.cap(1).toInt();
				createNeededTunesCount(num);

				QUrl url(sl.last());
				if(url.isLocalFile()) {
					list.at(num-1)->deleteLater();
					list[num-1] = Qomp::tuneFromFile(url.toLocalFile());
				}
				else
					list.at(num-1)->url = url.toString();
			}
		}
		else if (sl.first().startsWith("Length")) {
			QRegExp re("Length(\\d+)");
			int len = sl.last().toInt();
			if(len != -1 && re.indexIn(sl.first()) != -1) {
				int num = re.cap(1).toInt();
				createNeededTunesCount(num);
				if(list.at(num-1)->file.isEmpty())
					list.at(num-1)->duration = Qomp::durationSecondsToString(len);
			}
		}
		else if (sl.first().startsWith("Title")) {
			QRegExp re("Title(\\d+)");
			if(re.indexIn(sl.first()) != -1) {
				int num = re.cap(1).toInt();
				createNeededTunesCount(num);
				if(list.at(num-1)->file.isEmpty())
					list.at(num-1)->title = sl.last();
			}
		}
	}

	return list;
}

bool PlaylistParser::checkQompPlaylist() const
{
	return _mimeType.compare(PLAIN_TYPE, Qt::CaseInsensitive) == 0
			&& _file.endsWith(QStringLiteral("." PLAYLIST_EXTENTION));
}
