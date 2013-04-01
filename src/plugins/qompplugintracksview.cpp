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

#include "qompplugintracksview.h"
#include "qompplugintracksmodel.h"
#include "qompplugintypes.h"

#include <QKeyEvent>

QompPluginTracksView::QompPluginTracksView(QWidget *parent) :
	QListView(parent)
{
	connect(this, SIGNAL(activated(QModelIndex)), SLOT(itemActivated(QModelIndex)));
	connect(this, SIGNAL(clicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
}

void QompPluginTracksView::itemSelected(const QModelIndex &index)
{
	QompPluginTracksModel* m = qobject_cast<QompPluginTracksModel*>(model());
	if(m && index.isValid()) {
		emit tuneSelected(m->tune(index));
	}
}

void QompPluginTracksView::itemActivated(const QModelIndex &index)
{
	QompPluginTracksModel* m = qobject_cast<QompPluginTracksModel*>(model());
	if(m && index.isValid()) {
		emit tuneSelected(m->tune(index));
		m->setData(index, DataToggle);
	}
}

void QompPluginTracksView::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Space) {
		itemActivated(currentIndex());
		ke->accept();
		return;
	}

	QListView::keyPressEvent(ke);
}
