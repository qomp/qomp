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

#include "filesystemcommon.h"
#include "tune.h"
#include "common.h"

#include <QString>

#ifndef Q_OS_MAC
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#else
#include <tag/fileref.h>
#include <tag/tag.h>
#include <tag/audioproperties.h>
#endif

namespace Qomp {

static TagLib::FileRef getTaglibRef(const QString& file)
{
	return TagLib::FileRef(Qomp::fileName2TaglibFileName(file), true, TagLib::AudioProperties::Accurate);
}

Tune* tuneFromFile(const QString& file)
{
	Tune* tune = new Tune(false);
	tune->file = file;

	TagLib::FileRef ref = getTaglibRef(file);
	if(!ref.isNull()) {
		if(ref.tag()) {
			TagLib::Tag* tag = ref.tag();
			tune->artist = safeTagLibString2QString( tag->artist() );
			tune->album = safeTagLibString2QString( tag->album() );
			tune->title = safeTagLibString2QString( tag->title() );
			tune->trackNumber = QString::number( tag->track() );

			Qomp::loadCover(tune, ref.file());
		}

		if(ref.audioProperties()) {
			TagLib::AudioProperties *prop = ref.audioProperties();
			tune->duration = Qomp::durationSecondsToString( prop->length() );
			tune->bitRate = QString::number( prop->bitrate() );
		}
	}

	return tune;
}

bool getAudioInfo(const QString &file, qint64 *durationMiliSecs, int *bitrate)
{
	TagLib::FileRef ref = getTaglibRef(file);
	if(!ref.isNull() && ref.audioProperties()) {
		TagLib::AudioProperties *prop = ref.audioProperties();

		*bitrate = prop->bitrate();
		*durationMiliSecs = prop->length() * 1000;

		return true;
	}

	return false;
}

}
