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

#ifndef TAGHELPERS_H
#define TAGHELPERS_H

#include "libqomp_global.h"
#ifndef Q_OS_MAC
#include <taglib/tiostream.h>
#else
#include <tag/tiostream.h>
#endif

class Tune;

namespace TagLib {
class String;
class File;
}

namespace Qomp {

LIBQOMPSHARED_EXPORT QString safeTagLibString2QString(const TagLib::String& string);
LIBQOMPSHARED_EXPORT TagLib::FileName fileName2TaglibFileName(const QString& file);

LIBQOMPSHARED_EXPORT void loadCover(Tune* tune, TagLib::File* file);

}
#endif // TAGHELPERS_H
