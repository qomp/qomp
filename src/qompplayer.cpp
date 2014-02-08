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

#include "qompplayer.h"
#include "qompmetadataresolver.h"
#include "tune.h"

#ifdef HAVE_PHONON
#include "qompphononplayer.h"
#elif HAVE_QTMULTIMEDIA
#include "qompqtmultimediaplayer.h"
#endif

#include <QCoreApplication>
#include <QTimer>

QompPlayer* QompPlayer::instance_ = 0;


QompPlayer::QompPlayer() :
	QObject(QCoreApplication::instance())
{
}

void QompPlayer::setTune(Tune *tune)
{
	currentTune_ = tune;
	doSetTune();
	emit tuneChanged(tune);
}

QompPlayer *QompPlayer::instance()
{
	if(instance_ == 0) {
#ifdef HAVE_PHONON
		instance_ = new QompPhononPlayer;
#elif HAVE_QTMULTIMEDIA
		instance_ = new QompQtMultimediaPlayer;
#endif
	}

	return instance_;
}

Tune *QompPlayer::currentTune() const
{
	return currentTune_;
}

void QompPlayer::resolveMetadata(const QList<Tune*>& tunes)
{
	if(metaDataResolver())
		metaDataResolver()->resolve(tunes);
}
