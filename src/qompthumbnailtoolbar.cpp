/*
 * Copyright (C) 2017 Khryukin Evgeny
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

#include "qompthumbnailtoolbar.h"
#include "thememanager.h"

#include <QWinThumbnailToolButton>

QompThumbnailToolBar::QompThumbnailToolBar(QObject *p) :
	QWinThumbnailToolBar(p),
	_prev(new QWinThumbnailToolButton(this)),
	_play(new QWinThumbnailToolButton(this)),
	_next(new QWinThumbnailToolButton(this)),
	_state(Qomp::StateStopped)
{
	_prev->setToolTip(tr("Previous"));
	_play->setToolTip(tr("Play/Pause"));
	_next->setToolTip(tr("Next"));

	connect(_prev, &QWinThumbnailToolButton::clicked, this, &QompThumbnailToolBar::prev);
	connect(_play, &QWinThumbnailToolButton::clicked, this, &QompThumbnailToolBar::play);
	connect(_next, &QWinThumbnailToolButton::clicked, this, &QompThumbnailToolBar::next);

	updateIcons();

	addButton(_prev);
	addButton(_play);
	addButton(_next);

	connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &QompThumbnailToolBar::updateIcons);
}

void QompThumbnailToolBar::setCurrentState(Qomp::State state)
{
	if (_state == state)
		return;

	_state = state;

	switch (state) {
	case Qomp::StatePlaying: {
		_play->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/pause")));
		break;
	}
	default: {
		_play->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/play")));
		break;
	}
	}
}

void QompThumbnailToolBar::updateIcons()
{
	_prev->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/prev")));
	_play->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/play")));
	_next->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/next")));
}
