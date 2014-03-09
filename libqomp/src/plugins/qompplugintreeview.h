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

#ifndef QompPluginTreeView_H
#define QompPluginTreeView_H

#include <QTreeView>
#include "libqomp_global.h"

class LIBQOMPSHARED_EXPORT QompPluginTreeView : public QTreeView
{
	Q_OBJECT
public:
	/**
	* This class shows search results and designed for use with QompPluginTreeModel.
	* It's a tree of elements of artists - albums - songs.
	* Can select and unselect elements with mouse doubleclick
	* or Space and Return(Enter) keyboard keys.
	*/
	QompPluginTreeView(QWidget *parent = 0);
	
protected:
	void keyPressEvent(QKeyEvent *ke);
	void mouseDoubleClickEvent(QMouseEvent *e);

private:
	void itemActivated();
};

#endif // QompPluginTreeView_H
