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

#include "qompactionslist.h"
#include "options.h"
#include <QCoreApplication>

static const QString keyOptionPrefix = "qomp.shortcut.";

QompActionsList* QompActionsList::instance_ = 0;

QompActionsList::QompActionsList() :
	QObject(qApp)
{
	fillActionsList();
	restoreFromOptions();
}

QompActionsList *QompActionsList::instance()
{
	if(!instance_)
		instance_ = new QompActionsList;

	return instance_;
}

QKeySequence QompActionsList::getShortcut(QompActionsList::QompActionType type) const
{
	QMap<QString, QompAction>::const_iterator it = actions_.constBegin();
	while (it != actions_.constEnd()) {
		if((*it).type == type)
			return (*it).shortcut;

		++it;
	}

	return QKeySequence();
}

void QompActionsList::updateAction(QompActionsList::QompActionType type, const QKeySequence &key)
{
	QMap<QString, QompAction>::iterator it = actions_.begin();
	while (it != actions_.end()) {
		if((*it).type == type) {
			(*it).shortcut = key;
			break;
		}
		++it;
	}
}

const QList<QompActionsList::QompAction> QompActionsList::actions() const
{
	return actions_.values();
}

void QompActionsList::saveToOptions()
{
	foreach (const QompAction& act, actions()) {
		Options::instance()->setOption(keyOptionPrefix + actions_.key(act), act.shortcut);
	}
}

void QompActionsList::restoreFromOptions()
{
	QMap<QString, QompAction>::iterator it = actions_.begin();
	while (it != actions_.end()) {
		(*it).shortcut = Options::instance()->getOption(keyOptionPrefix + actions_.key(*it), (*it).shortcut).value<QKeySequence>();
		++it;
	}
}

void QompActionsList::fillActionsList()
{
	actions_.insert("play", QompAction(ActPlay, tr("Play"), QKeySequence("Space")));
	actions_.insert("stop", QompAction(ActStop, tr("Stop"), QKeySequence("S")));
	actions_.insert("next", QompAction(ActNext, tr("Next"), QKeySequence("F2")));
	actions_.insert("prev", QompAction(ActPrevious, tr("Previous"), QKeySequence("F1")));
	actions_.insert("open", QompAction(ActOpen, tr("Open"), QKeySequence("Ctrl+L")));
	actions_.insert("settings", QompAction(ActSettings, tr("Options"), QKeySequence("Ctrl+p")));
	actions_.insert("repeatall", QompAction(ActRepeatAll, tr("Repeat All"), QKeySequence("R")));
	actions_.insert("shuffle", QompAction(ActShuffle, tr("Shuffle"), QKeySequence("F")));
	actions_.insert("mute", QompAction(ActMute, tr("Mute"), QKeySequence("V")));
	actions_.insert("clearplaylist", QompAction(ActClearPlaylist, tr("Clear Playlist"), QKeySequence("Ctrl+Delete")));
	actions_.insert("removetune", QompAction(ActRemoveTune, tr("Remove selected"), QKeySequence("Delete")));
}
