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

#ifndef QOMPMENU_MOBILE_H
#define QOMPMENU_MOBILE_H

#include <QObject>
class Tune;
class QAction;
class QQuickItem;
class QQmlComponent;

class QompPluginsMenu : public QObject
{
	Q_OBJECT
public:
	explicit QompPluginsMenu(QObject *parent = 0);
	~QompPluginsMenu();
	void popup();

signals:
	void tunes(const QList<Tune*>&);

private slots:
	void actionActivated(QAction *sender);

private:
	QQuickItem* root_;
	QQmlComponent* comp_;
};

#endif // QOMPMENU_MOBILE_H
