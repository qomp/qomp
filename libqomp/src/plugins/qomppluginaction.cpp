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

#include "qomppluginaction.h"
#include "tune.h"

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

#ifndef Q_OS_ANDROID
#include <QAction>
#endif

QompPluginAction::QompPluginAction(const QIcon &ico,
				   const QString &text,
				   QObject *receiver,
				   const char *slot,
				   QObject *parent) :
	QObject(parent),
	receiver_(receiver),
	slot_(slot),
#ifndef Q_OS_ANDROID
	action_(new QAction(ico, text, this))
#else
	text_(text),
	icon_(ico)
#endif
{
#ifndef Q_OS_ANDROID
	connect(action_, &QAction::triggered, this, &QompPluginAction::getTunes);
	connect(action_, &QAction::destroyed, this, &QompPluginAction::deleteLater);
#endif
}

QompPluginAction::~QompPluginAction()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompPluginAction destroyed";
#endif
}

void QompPluginAction::getTunes()
{
	if(receiver_)
		QMetaObject::invokeMethod(receiver_, slot_, Qt::DirectConnection, Q_ARG(QompPluginAction*, this));
}

void QompPluginAction::setTunesReady(const QList<Tune *> &tunes)
{
	emit tunesReady(tunes);
}

QString QompPluginAction::text() const
{
#ifndef Q_OS_ANDROID
	return action_->text();
#else
	return text_;
#endif
}
#ifndef Q_OS_ANDROID
QAction *QompPluginAction::action() const
{
	return action_;
}
#endif
