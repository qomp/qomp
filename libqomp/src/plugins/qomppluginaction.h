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

#ifndef QOMPPLUGINACTION_H
#define QOMPPLUGINACTION_H

#include <QIcon>
#include "libqomp_global.h"

class Tune;
#ifdef HAVE_QT6
Q_MOC_INCLUDE("tune.h")
#endif
#ifndef Q_OS_ANDROID
class QAction;
#endif

class LIBQOMPSHARED_EXPORT QompPluginAction : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text() NOTIFY textChanged())

public:
	QompPluginAction(const QIcon& ico, const QString& text, QObject* receiver, const char* slot, QObject *parent = nullptr);
	~QompPluginAction();

	QString text() const;
#ifndef Q_OS_ANDROID
	QAction* action() const;
#endif

public slots:
	void getTunes();
	void setTunesReady(const QList<Tune*>& tunes);

signals:
	void textChanged();
	void tunesReady(const QList<Tune*>&);

private:
	QObject* receiver_;
	const char* slot_;

#ifndef Q_OS_ANDROID
	QAction* action_;
#else
	QString text_;
	QIcon icon_;
#endif
};

#endif // QOMPPLUGINACTION_H
