/*
 * Copyright (C) 2013  Khryukin Evgeny
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

#ifndef QOMPPLAYLISTVIEW_H
#define QOMPPLAYLISTVIEW_H

#include <QListView>

#include "tune.h"

class QompPlaylistView : public QListView
{
	Q_OBJECT
public:
	explicit QompPlaylistView(QWidget *parent = 0);
	
protected:
	virtual void startDrag(Qt::DropActions supportedActions);
	virtual void dropEvent(QDropEvent *e);

private:
	TuneList selected_;
	
};

#endif // QOMPPLAYLISTVIEW_H
