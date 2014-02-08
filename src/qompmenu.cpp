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
#include "tune.h"

#include <QApplication>
#include <QClipboard>

QompMenu::QompMenu(QWidget *parent) :
	QMenu(parent)
{
	connect(this, SIGNAL(aboutToShow()), SLOT(menuAboutToShow()));
}

QompMenu::QompMenu(const QString &name, QWidget *parent) :
	QMenu(name, parent)
{
	connect(this, SIGNAL(aboutToShow()), SLOT(menuAboutToShow()));
}

void QompMenu::leaveEvent(QEvent *e)
{
	QMenu::leaveEvent(e);

	hide();
	deleteLater();
}

void QompMenu::menuAboutToShow()
{
	qDeleteAll(actions());
	actions().clear();

	buildMenu();
}




QompGetTunesMenu::QompGetTunesMenu(QWidget *parent) :
	QompMenu(parent)
{
}

QompGetTunesMenu::QompGetTunesMenu(const QString &name, QWidget *parent) :
	QompMenu(name, parent)
{
}

void QompGetTunesMenu::actionActivated()
{
	QAction* act = static_cast<QAction*>(sender());
	QList<Tune*> t = PluginManager::instance()->getTune(act->text());
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
	QompMenu(p)
{
}

void QompMainMenu::buildMenu()
{
	QAction* act = addAction(tr("Toggle Visibility"), this, SIGNAL(actToggleVisibility()));
	act->setParent(this);

	QompGetTunesMenu *open = new QompGetTunesMenu(tr("Open"), this);
	connect(open, SIGNAL(tunes(QList<Tune*>)), SIGNAL(tunes(QList<Tune*>)));
	addMenu(open);

	act = addAction(tr("Settings"), this, SIGNAL(actDoOptions()));
	act->setParent(this);
	addSeparator();

	QMenu* helpMenu = new QMenu(tr("Help"), this);
	act = helpMenu->addAction(tr("About qomp"), this, SIGNAL(actAbout()));
	act->setParent(helpMenu);
	act = helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));
	act->setParent(helpMenu);
	act = helpMenu->addAction(tr("Check for updates"), this, SIGNAL(actCheckUpdates()));
	act->setParent(helpMenu);
	addMenu(helpMenu);

	act = addAction(tr("Exit"), this, SIGNAL(actExit()));
	act->setParent(this);
}


QompTrackMenu::QompTrackMenu(Tune *tune, QWidget *p) :
	QompMenu(p),
	tune_(tune)
{
}

void QompTrackMenu::actRemoveActivated()
{
	emit removeTune(tune_);
}

void QompTrackMenu::actCopyUrlActivated()
{
	qApp->clipboard()->setText(tune_->getUrl().toString());
}

void QompTrackMenu::actSaveActivated()
{
	emit saveTune(tune_);
}

void QompTrackMenu::actToggleActivated()
{
	emit togglePlayState(tune_);
}

void QompTrackMenu::buildMenu()
{
	QAction* act = addAction(tr("Play/Pause"), this, SLOT(actToggleActivated()));
	act->setParent(this);

	act = addAction(tr("Remove"), this, SLOT(actRemoveActivated()));
	act->setParent(this);

	if(!tune_->url.isEmpty()) {
		act = addAction(tr("Copy URL"), this, SLOT(actCopyUrlActivated()));
		act->setParent(this);
	}
	if(tune_->canSave()) {
		act = addAction(tr("Save File"), this, SLOT(actSaveActivated()));
		act->setParent(this);
	}
}


QompRemoveTunesMenu::QompRemoveTunesMenu(QWidget *p) :
	QompMenu(p)
{
}

void QompRemoveTunesMenu::buildMenu()
{
	QAction *act = addAction(tr("Remove All"), this, SIGNAL(removeAll()));
	act->setParent(this);
	act = addAction(tr("Remove Selected"), this, SIGNAL(removeSelected()));
	act->setParent(this);
}
