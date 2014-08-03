/*
 * Copyright (C) 2013  Khryukin Evgeny
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

#ifndef COMMON_H
#define COMMON_H

#include <QString>

#include "libqomp_global.h"

//namespace TagLib {
//class String;
//}

namespace Qomp {

LIBQOMPSHARED_EXPORT QString encodePassword(const QString& str, const QString &key);
LIBQOMPSHARED_EXPORT QString decodePassword(const QString& str, const QString &key);

LIBQOMPSHARED_EXPORT QString durationSecondsToString(uint sec);
LIBQOMPSHARED_EXPORT QString durationMiliSecondsToString(qint64 ms);
LIBQOMPSHARED_EXPORT uint durationStringToSeconds(const QString& dur);

LIBQOMPSHARED_EXPORT QString unescape(const QString& escaped);
//LIBQOMPSHARED_EXPORT QString fixEncoding(const TagLib::String& encoded);

LIBQOMPSHARED_EXPORT QString cacheDir();
LIBQOMPSHARED_EXPORT QString dataDir();

enum /*LIBQOMPSHARED_EXPORT*/ State { StateUnknown,StateStopped,StatePaused,StatePlaying,StateError,StateLoading,StateBuffering };

} //namespace Qomp


#ifdef DEBUG_OUTPUT
#include <QtDebug>

LIBQOMPSHARED_EXPORT QDebug operator<<(QDebug dbg, Qomp::State value);
#endif

#endif // COMMON_H
