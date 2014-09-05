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

#ifndef QOMPQMLENGINE_H
#define QOMPQMLENGINE_H

#include <QQmlApplicationEngine>
#include "libqomp_global.h"

class QQuickItem;
class QQuickWindow;
class Scaler;

class LIBQOMPSHARED_EXPORT QompQmlEngine : public QQmlApplicationEngine
{
	Q_OBJECT
public:
	static QompQmlEngine *instance();
	~QompQmlEngine();

	QQuickItem* createItem(const QUrl& url);
	void addItem(QQuickItem* item);
	void removeItem();

private slots:
	void itemDeleted();

protected:
	bool eventFilter(QObject *o, QEvent *e);

private:
	static QompQmlEngine *instance_;
	QompQmlEngine();

	QQuickWindow* window_;
	Scaler* scaler_;
};

#endif // QOMPQMLENGINE_H
