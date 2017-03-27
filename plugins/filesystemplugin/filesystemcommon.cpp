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
#include <QImage>
#include <QFileInfo>

#ifndef Q_OS_MAC
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/mpegfile.h>
#else
#include <tag/fileref.h>
#include <tag/tag.h>
#include <tag/audioproperties.h>
#include <tag/id3v2tag.h>
#include <tag/attachedpictureframe.h>
#include <tag/mpegfile.h>
#endif

namespace Qomp {

static TagLib::FileRef getTaglibRef(const QString& file)
{
	TagLib::String str( file.toUtf8().constData(), TagLib::String::UTF8 );
#ifdef Q_OS_WIN
	TagLib::FileName fname(str.toCWString());
#else
	TagLib::FileName fname(str.toCString(true));
#endif
	return TagLib::FileRef(fname, true, TagLib::AudioProperties::Accurate);
}

static bool searchLocalCover(Tune* t)
{
	static const QStringList coverList{"front", "cover", "albumart"};
	static const QStringList coverExt{"jpg", "jpeg", "png"};
	for(const QString& cover: coverList)
		for(const QString& ext: coverExt) {
			const QString file = QFileInfo(t->file).absolutePath() + "/" + cover + "." + ext;
			if(QFile::exists(file)) {
				QImage i(file);
				if(!i.isNull()) {
					t->setCover(i);
					return true;
				}
			}
		}

	return false;
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

			if (!searchLocalCover(tune)) {
				auto mpeg = dynamic_cast<TagLib::MPEG::File*>(ref.file());

				if(mpeg) {
					TagLib::ID3v2::Tag* tag2 = mpeg->ID3v2Tag();
					if(tag2) {
						TagLib::ID3v2::FrameList frameList = tag2->frameList("APIC");
						if(!frameList.isEmpty()) {
							TagLib::ID3v2::AttachedPictureFrame *coverImg = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());

							QImage cover;
							cover.loadFromData((const uchar *) coverImg->picture().data(), coverImg->picture().size());
							tune->setCover(cover);
						}
					}
				}
			}
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
