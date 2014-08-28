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

#include "qompmenu_mobile.h"
#include "qomppluginaction.h"
#include "qompqmlengine.h"

#include <QQuickItem>
#include <QQmlComponent>

QompPluginsMenu::QompPluginsMenu(QObject *parent) :
	QObject(parent)
{
	QompQmlEngine* e = QompQmlEngine::instance();
	comp_ = new QQmlComponent(e, QUrl("qrc:///qml/OpenTuneMenu.qml"));
	root_ = static_cast<QQuickItem*>(comp_->create(e->rootContext()));
}

QompPluginsMenu::~QompPluginsMenu()
{
	delete root_;
	delete comp_;
}

void QompPluginsMenu::popup()
{
	QMetaObject::invokeMethod(root_, "popup");
}

void QompPluginsMenu::actionActivated(QAction *sender)
{
	deleteLater();
	QompPluginAction* act = static_cast<QompPluginAction*>(sender);
	QList<Tune*> t = act->getTunes();
	if(!t.isEmpty())
		emit tunes(t);
}
