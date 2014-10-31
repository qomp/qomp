/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "gettuneurlhelper.h"
#include "tune.h"

#ifdef HAVE_QT5
#include <QtConcurrent>
#else
#include <QtConcurrentRun>
#endif

GetTuneUrlHelper::GetTuneUrlHelper(QObject *target, const char *slot, QObject *parent) :
	QObject(parent),
	target_(target),
	slot_(slot),
	blocked_(false)
{
}

QFutureWatcher<QUrl> *GetTuneUrlHelper::getTuneUrlAsynchronously(Tune *t)
{
	QFutureWatcher<QUrl>* watcher = new QFutureWatcher<QUrl>(this);
	connect(watcher, SIGNAL(finished()), SLOT(urlFinished()));
	QFuture<QUrl> f = QtConcurrent::run(t, &Tune::getUrl);
	watcher->setFuture(f);
	return watcher;
}

bool GetTuneUrlHelper::blocked() const
{
	return blocked_;
}

void GetTuneUrlHelper::setBlocked(bool b)
{
	blocked_ = b;
}

void GetTuneUrlHelper::urlFinished()
{
	QFutureWatcher<QUrl>* watcher = static_cast<QFutureWatcher<QUrl>*>(sender());
	QFuture<QUrl> f = watcher->future();
	watcher->deleteLater();
	QUrl url = f.result();
	if(!blocked_)
		QMetaObject::invokeMethod(target_, slot_, Q_ARG(QUrl, url));
	deleteLater();
}
