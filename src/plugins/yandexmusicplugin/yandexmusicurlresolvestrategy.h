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

#ifndef YANDEXMUSICURLRESOLVESTRATEGY_H
#define YANDEXMUSICURLRESOLVESTRATEGY_H

#include "tune.h"
#include <QObject>

class QMutex;
class QEventLoop;
class QTimer;

class YandexMusicURLResolveStrategy : public QObject, public TuneURLResolveStrategy
{
	Q_OBJECT
public:
	static YandexMusicURLResolveStrategy* instance();
	~YandexMusicURLResolveStrategy();

	virtual QUrl getUrl(const Tune *t);
	
private slots:
	void tuneUrlFinishedStepOne();
	void tuneUrlFinishedStepTwo();
	void timeout();

private:
	explicit YandexMusicURLResolveStrategy();
	static YandexMusicURLResolveStrategy* instance_;
	QUrl url_;
	QEventLoop* loop_;
	Tune *tune_;
	mutable QMutex* mutex_;
	QTimer* timer_;
};

#endif // YANDEXMUSICURLRESOLVESTRATEGY_H
