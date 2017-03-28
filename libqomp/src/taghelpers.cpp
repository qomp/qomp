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

#include "taghelpers.h"
#include "defines.h"
#include "options.h"
#include "tune.h"

#include <QTextCodec>
#include <QFileInfo>
#ifndef Q_OS_MAC
#include <taglib/tstring.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/mpegfile.h>
#include <taglib/fileref.h>
#else
#include <tag/tstring.h>
#include <tag/id3v2tag.h>
#include <tag/attachedpictureframe.h>
#include <tag/mpegfile.h>
#include <tag/fileref.h>
#endif

namespace Qomp {

QString safeTagLibString2QString(const TagLib::String& string)
{
	QString ret;

	if(string.isAscii()) {
		ret = QString::fromLatin1(string.toCString(false));
	}
	else if(!string.isLatin1()) {
		ret = QString::fromUtf8(string.toCString(true));
	}
	else {
		QByteArray ba(string.toCString(false));
		ret = QString(ba);

		const QByteArray decoding = Options::instance()->getOption(OPTION_DEFAULT_ENCODING).toByteArray();
		QTextCodec *tc = QTextCodec::codecForName(decoding);
		if(tc) {
			ret = tc->toUnicode(ba);
		}
	}

	return ret;
}

static bool searchLocalCover(Tune* t)
{
	if(t->file.isEmpty())
		return false;

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

void loadCover(Tune *tune, TagLib::File *file)
{
	if (!searchLocalCover(tune)) {
		auto mpeg = dynamic_cast<TagLib::MPEG::File*>(file);

		if(mpeg) {
			TagLib::ID3v2::Tag* tag2 = mpeg->ID3v2Tag();
			if(tag2) {
				TagLib::ID3v2::FrameList frameList = tag2->frameList("APIC");
				if(!frameList.isEmpty()) {
					for(unsigned int i = 0; i < frameList.size(); ++i) {
						auto *coverImg = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList[i]);
						if(coverImg) {
							QImage cover;
							if(cover.loadFromData(reinterpret_cast<const uchar*>(coverImg->picture().data()),
									      coverImg->picture().size()))
							{
								tune->setCover(cover);
								break;
							}
						}
					}
				}
			}
		}
	}
}

TagLib::FileRef fileName2TaglibRef(const QString &file, bool getAudioProps)
{
	TagLib::String str( file.toUtf8().constData(), TagLib::String::UTF8 );
#ifdef Q_OS_WIN
	TagLib::FileName fname(str.toCWString());
#else
	TagLib::FileName fname(str.toCString(true));
#endif
	if(getAudioProps)
		return TagLib::FileRef(fname, true, TagLib::AudioProperties::Accurate);
	else
		return TagLib::FileRef(fname, false);
}

}
