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

#include "qompmenu.h"
#include "pluginmanager.h"

QompGetTunesMenu::QompGetTunesMenu(QWidget *parent) :
	QMenu(parent)
{
	buildMenu();
}
QompGetTunesMenu::QompGetTunesMenu(const QString &name, QWidget *parent) :
	QMenu(name, parent)
{
	buildMenu();
}

void QompGetTunesMenu::actionActivated()
{
	QAction* act = static_cast<QAction*>(sender());
	TuneList t = PluginManager::instance()->getTune(act->text());
	if(!t.isEmpty())
		emit tunes(t);

}

void QompGetTunesMenu::buildMenu()
{
	foreach(const QString& name, PluginManager::instance()->tunePlugins()) {
		if(PluginManager::instance()->isPluginEnabled(name)) {
			QAction* act = addAction(name, this, SLOT(actionActivated()));
			act->setParent(this);
		}
	}
}
