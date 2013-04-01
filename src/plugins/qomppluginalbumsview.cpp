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

#include "qomppluginalbumsview.h"
#include "qomppluginalbumsmodel.h"
#include "qompplugintypes.h"

#include <QKeyEvent>
#include <QApplication>
#include <QTimer>

QompPluginAlbumsView::QompPluginAlbumsView(QWidget *parent) :
	QTreeView(parent)
{
	setHeaderHidden(true);
	setItemsExpandable(true);
}

void QompPluginAlbumsView::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Space) {
		itemActivated();
		ke->accept();
		return;
	}

	QTreeView::keyPressEvent(ke);
}

void QompPluginAlbumsView::mousePressEvent(QMouseEvent *e)
{
	lastClickTime_ = QTime::currentTime();
	QModelIndex i = indexAt(e->pos());
	if(i.isValid()) {
		setCurrentIndex(i);
		QompPluginAlbumsModel* m = qobject_cast<QompPluginAlbumsModel*>(model());
		if(m) {
			if(m->isAlbum(i))
				emit albumSelected(m->album(i));
			else
				emit tuneSelected(m->tune(i));
		}
		QTimer::singleShot(QApplication::doubleClickInterval(), this, SLOT(itemSelected()));
		return;
	}

	return QTreeView::mousePressEvent(e);
}

void QompPluginAlbumsView::mouseDoubleClickEvent(QMouseEvent *e)
{
	lastClickTime_ = QTime::currentTime();
	QModelIndex i = indexAt(e->pos());
	if(i.isValid()) {
		setCurrentIndex(i);
		itemActivated();
		return;
	}
	return QTreeView::mouseDoubleClickEvent(e);
}

void QompPluginAlbumsView::itemSelected()
{
	if(lastClickTime_.msecsTo(QTime::currentTime()) < QApplication::doubleClickInterval())
		return;

	QModelIndex i = currentIndex();
	if(i.isValid()) {
		if(isExpanded(i))
			collapse(i);
		else {
			expand(i);
		}
	}
}

void QompPluginAlbumsView::itemActivated()
{
	QModelIndex i = currentIndex();
	if(i.isValid()) {
		QompPluginAlbumsModel* m = qobject_cast<QompPluginAlbumsModel*>(model());
		if(m) {
			m->setData(i, DataToggle);
		}
	}
}
