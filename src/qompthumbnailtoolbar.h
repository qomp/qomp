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

#ifndef QOMPTHUMBNAILTOOLBAR_H
#define QOMPTHUMBNAILTOOLBAR_H

#include <QWinThumbnailToolBar>

#include "common.h"

class QWinThumbnailToolButton;

class QompThumbnailToolBar : public QWinThumbnailToolBar
{
	Q_OBJECT
public:
	QompThumbnailToolBar(QObject* p = nullptr);

	void setCurrentState(Qomp::State state);

signals:
	void next();
	void play();
	void prev();

private slots:
	void updateIcons();

private:
	QWinThumbnailToolButton* _prev;
	QWinThumbnailToolButton* _play;
	QWinThumbnailToolButton* _next;
	Qomp::State _state;
};

#endif // QOMPTHUMBNAILTOOLBAR_H
