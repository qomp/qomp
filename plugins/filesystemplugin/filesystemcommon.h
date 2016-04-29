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

#ifndef FILESYSTEMCOMMON_H
#define FILESYSTEMCOMMON_H

class Tune;
class QString;

#include <QtGlobal>


namespace Qomp {

Tune* tuneFromFile(const QString& file);

bool getAudioInfo(const QString& file, qint64* durationMiliSecs, int* bitrate);

}

#endif // FILESYSTEMCOMMON_H
