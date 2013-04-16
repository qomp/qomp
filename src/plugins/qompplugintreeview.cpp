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

#include "qompplugintreeview.h"
#include "qompplugintypes.h"

#include <QKeyEvent>


QompPluginTreeView::QompPluginTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setHeaderHidden(true);
	setItemsExpandable(true);
}

void QompPluginTreeView::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Space) {
		itemActivated();
		ke->accept();
		return;
	}

	QTreeView::keyPressEvent(ke);
}

void QompPluginTreeView::mouseDoubleClickEvent(QMouseEvent *e)
{
	QModelIndex i = indexAt(e->pos());
	if(i.isValid()) {
		setCurrentIndex(i);
		itemActivated();
		return;
	}
	return QTreeView::mouseDoubleClickEvent(e);
}

void QompPluginTreeView::itemActivated()
{
	QModelIndex i = currentIndex();
	if(i.isValid()) {
		model()->setData(i, Qomp::DataToggle, Qt::CheckStateRole);
	}
}
