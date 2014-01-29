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

#include <QApplication>

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


QompMainMenu::QompMainMenu(QWidget *p) :
	QMenu(p)
{
	buildMenu();
}

void QompMainMenu::buildMenu()
{
	addAction(tr("Toggle Visibility"), this, SIGNAL(actToggleVisibility()));

	QompGetTunesMenu *open = new QompGetTunesMenu(tr("Open"), this);
	connect(open, SIGNAL(tunes(TuneList)), SIGNAL(tunes(TuneList)));
	addMenu(open);

	addAction(tr("Settings"), this, SIGNAL(actDoOptions()));
	addSeparator();

	QMenu* helpMenu = new QMenu(tr("Help"), this);
	helpMenu->addAction(tr("About qomp"), this, SIGNAL(actAbout()));
	helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));
	helpMenu->addAction(tr("Check for updates"), this, SIGNAL(actCheckUpdates()));
	addMenu(helpMenu);

	addAction(tr("Exit"), this, SIGNAL(actExit()));
}
