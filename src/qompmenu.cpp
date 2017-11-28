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
#include "qomppluginaction.h"
#include "thememanager.h"
#include "qompplaylistmodel.h"

#include <QApplication>
#include <QDesktopServices>


QompMenu::QompMenu(const QString &name, QWidget *parent) :
	QMenu(name, parent)
{
	connect(this, &QompMenu::aboutToShow, this, &QompMenu::menuAboutToShow);
}

QompMenu::~QompMenu()
{
	clearMenu();
}

void QompMenu::clearMenu()
{
	foreach (QAction* act, actions()) {
		delete act->menu();
		delete act;
	}
}

void QompMenu::menuAboutToShow()
{
	clearMenu();
	buildMenu();
}




QompGetTunesMenu::QompGetTunesMenu(const QString &name, QWidget *parent) :
	QompMenu(name, parent)
{
	connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &QompGetTunesMenu::updateIcons);
	updateIcons();
}

void QompGetTunesMenu::buildMenu()
{
	foreach(QompPluginAction* act, PluginManager::instance()->tunesActions()) {
		addAction(act->action());
		if(act->action()->menu()) {
			for(QAction* a: act->action()->menu()->actions()) {
				connect(static_cast<QompPluginAction*>(a->parent()),
					&QompPluginAction::tunesReady, this, &QompGetTunesMenu::tunes);
			}
		}
		connect(act, &QompPluginAction::tunesReady, this, &QompGetTunesMenu::tunes);
	}
}

void QompGetTunesMenu::updateIcons()
{
	setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/add"));
}




QompMainMenu::QompMainMenu(QWidget *p) :
	QompMenu("", p),
	_tunesMenu(new QompGetTunesMenu(tr("Open"), this))
{
	connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &QompMainMenu::updateIcons);
	connect(_tunesMenu, &QompGetTunesMenu::tunes, this, &QompMainMenu::tunes);
	buildMenu();
}

QompGetTunesMenu *QompMainMenu::tunesMenu() const
{
	return _tunesMenu;
}

void QompMainMenu::menuAboutToShow()
{
}

void QompMainMenu::updateIcons()
{
	_helpMenu->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/help"));
	_actToggle->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/toggle"));
	_actOptions->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/options"));
	_actHome->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/home"));
	_actAboutQt->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/info"));
	_actBugs->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/bugreport"));
	_actUpdates->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/updates"));
	_actQuit->setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/close"));
}

void QompMainMenu::buildMenu()
{
	_actToggle = addAction(tr("Toggle Visibility"), this, SIGNAL(actToggleVisibility()));
	_actToggle->setParent(this);

	addMenu(_tunesMenu);

	_actOptions = addAction(tr("Settings"), this, SIGNAL(actDoOptions()));
	_actOptions->setParent(this);
	addSeparator();

	_helpMenu = new QMenu(tr("Help"), this);

	_actHome = _helpMenu->addAction(tr("About qomp"), this, SIGNAL(actAbout()));
	_actHome->setParent(_helpMenu);

	_actAboutQt = _helpMenu->addAction(tr("About Qt"), qApp, SLOT(aboutQt()));
	_actAboutQt->setParent(_helpMenu);

	_actBugs = _helpMenu->addAction(tr("Report a bug"), this, SIGNAL(actReportBug()));
	_actBugs->setParent(_helpMenu);

	_actUpdates = _helpMenu->addAction(tr("Check for updates"), this, SIGNAL(actCheckUpdates()));
	_actUpdates->setParent(_helpMenu);

	addMenu(_helpMenu);

	_actQuit = addAction(tr("Exit"), this, SIGNAL(actExit()));
	_actQuit->setParent(this);

	updateIcons();
}


QompTrackMenu::QompTrackMenu(const QModelIndexList &list, QWidget *p) :
	QompMenu("", p),
	list_(list)
{
}

void QompTrackMenu::actRemoveActivated()
{
	emit removeTune(list_);
}

void QompTrackMenu::actCopyUrlActivated()
{
	emit copyUrl(list_[0].data(QompPlayListModel::TuneRole).value<Tune*>());
}

void QompTrackMenu::actSaveActivated()
{
	emit saveTune(list_);
}

void QompTrackMenu::actToggleActivated()
{
	emit togglePlayState(list_[0].data(QompPlayListModel::TuneRole).value<Tune*>());
}

void QompTrackMenu::actOpenDirectActivated()
{
	QDesktopServices::openUrl(list_[0].data(QompPlayListModel::TuneRole).value<Tune*>()->directUrl);
}

void QompTrackMenu::buildMenu()
{
	if (list_.size() == 0)
		return;

	QModelIndex first;
	if(list_.size() == 1)
		first = list_.at(0);

	bool hasUrl, hasSave;
	hasUrl = hasSave = false;

	for(const QModelIndex& index: list_) {
		if(index.data(QompPlayListModel::CanDownloadRole).toBool()) {
			hasSave = true;
		}

		if(!index.data(QompPlayListModel::URLRole).toString().isEmpty()) {
			hasUrl = true;
		}

		if(hasSave && hasUrl)
			break;
	}

	QAction* act;
	if(first.isValid()) {
		act = addAction(ThemeManager::instance()->getIconFromTheme(":/icons/play"), tr("Play/Pause"), this, SLOT(actToggleActivated()));
		act->setParent(this);

		if(hasUrl) {
			act = addAction(ThemeManager::instance()->getIconFromTheme(":/icons/ok"), tr("Copy URL"), this, SLOT(actCopyUrlActivated()));
			act->setParent(this);
		}

		const QString direct = first.data(QompPlayListModel::TuneRole).value<Tune*>()->directUrl;
		if(!direct.isEmpty()) {
			act = addAction(ThemeManager::instance()->getIconFromTheme(":/icons/updates"), tr("Open At Browser"), this, SLOT(actOpenDirectActivated()));
			act->setParent(this);
		}
	}

	act = addAction(ThemeManager::instance()->getIconFromTheme(":/icons/delete"), tr("Remove"), this, SLOT(actRemoveActivated()));
	act->setParent(this);


	if(hasSave) {
		act = addAction(ThemeManager::instance()->getIconFromTheme(":/icons/save"), tr("Save File(s)"), this, SLOT(actSaveActivated()));
		act->setParent(this);
	}
}


QompRemoveTunesMenu::QompRemoveTunesMenu(QWidget *p) :
	QompMenu("", p)
{
}

void QompRemoveTunesMenu::buildMenu()
{
	QAction *act = addAction(tr("Remove All"), this, SIGNAL(removeAll()));
	act->setParent(this);
	act = addAction(tr("Remove Selected"), this, SIGNAL(removeSelected()));
	act->setParent(this);
}
