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

#include "cueparser.h"
#include "tune.h"
#include "common.h"
#include "filesystemcommon.h"
#include "options.h"
#include "defines.h"

extern "C" {
#include "libcue.h"
}

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTextCodec>

CueParser::CueParser(const QString &fname)
{
	QFile f(fname);
	if(f.open(QFile::ReadOnly)) {
		QFileInfo fi(f);
		_path = fi.absolutePath();

		QTextStream stream(&f);

		const QByteArray decoding = Options::instance()->getOption(OPTION_DEFAULT_ENCODING).toByteArray();
		QTextCodec *tc = QTextCodec::codecForName(decoding);
		if(tc)
			stream.setCodec(tc);


		stream.setAutoDetectUnicode(true);

		_cd = cue_parse_string(stream.readAll().toUtf8().constData());
	}
}

CueParser::~CueParser()
{
	if(_cd)
		cd_delete(_cd);
}

QList<Tune *> CueParser::parseTunes(const QString &fname)
{
	QList<Tune*> tunes;

	CueParser cp(fname);

	if(cp.isValid()) {
		int c = cp.filesCount();
		for(int i = 0; i < c; ++ i) {
			tunes.append(cp.tune(i));
		}
	}

	return tunes;
}

bool CueParser::isValid() const
{
	return _cd != nullptr;
}

int CueParser::filesCount() const
{
	if(isValid())
		return cd_get_ntrack(_cd);

	return 0;
}

Tune *CueParser::tune(int index) const
{
	if(index >= filesCount() || !isValid())
		return nullptr;

	Track *track = cd_get_track(_cd, index + 1);
	Cdtext *ct = track_get_cdtext(track);

	QString artist(QString::fromUtf8(cdtext_get(PTI_PERFORMER, ct)));
	if(artist.isEmpty())
		artist = this->artist();

	QString title(QString::fromUtf8( cdtext_get(PTI_TITLE, ct)));
	QString fname(QString::fromUtf8(track_get_filename(track)));

	long start =  framesLenToMiliSecs(track_get_start(track));
	long len   =  framesLenToMiliSecs(track_get_length(track));

	Tune* t = new Tune(false);
	t->artist = artist;
	t->album = album();
	t->title = title;
	t->file = _path + '/' + fname;
	t->trackNumber = QString::number(index + 1);
	t->start = start;
	t->length = len;

	qint64 dur;
	int bitrate;
	if(Qomp::getAudioInfo(t->file, &dur, &bitrate)) {
		t->bitRate = QString::number(bitrate);
		if(t->length <= 0) {
			t->length = dur - start;
		}
	}

	t->duration = Qomp::durationMiliSecondsToString(t->length);

	return t;
}

const QString &CueParser::artist() const
{
	if(_artist.isNull() && isValid()) {
		Cdtext *ct = cd_get_cdtext(_cd);
		_artist = QString::fromUtf8( cdtext_get(PTI_PERFORMER, ct) );
	}

	return _artist;
}

const QString &CueParser::album() const
{
	if(_album.isNull() && isValid()) {
		Cdtext *ct = cd_get_cdtext(_cd);
		_album = QString::fromUtf8( cdtext_get(PTI_TITLE, ct) );
	}

	return _album;
}

qint64 CueParser::framesLenToMiliSecs(long frames)
{
	//1 frame = 1/75 sec
	return frames * 1000 / 75;
}

