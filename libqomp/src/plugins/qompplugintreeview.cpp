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

#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QTimer>


QompPluginTreeView::QompPluginTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setHeaderHidden(true);
	setItemsExpandable(true);
	setUniformRowHeights(true);
	setSelectionMode(QTreeView::ExtendedSelection);

	connect(this, &QompPluginTreeView::expanded, this, &QompPluginTreeView::itemActivated);
	connect(this, &QompPluginTreeView::clicked,  this, &QompPluginTreeView::itemActivated);
}

void QompPluginTreeView::keyPressEvent(QKeyEvent *ke)
{
	if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Space) {
		activateItem();
		ke->accept();
		return;
	}

	QTreeView::keyPressEvent(ke);
}

void QompPluginTreeView::mouseDoubleClickEvent(QMouseEvent *e)
{
	const QModelIndex i = indexAt(e->pos());
	const QRect r = branchIndicatorRectAt(i);
	if(i.isValid() && (i.flags() & Qt::ItemIsUserCheckable) && r.contains(e->pos())) {
		setCurrentIndex(i);
		activateItem();
		e->accept();
		return;
	}

	QTreeView::mouseDoubleClickEvent(e);
}

void QompPluginTreeView::mouseMoveEvent(QMouseEvent *e)
{
	//without this QTreeView accepts the event so it is not propogadet to paren widget
	if(e->buttons() == Qt::NoButton)
		e->ignore();
	else
		QTreeView::mouseMoveEvent(e);
}

void QompPluginTreeView::contextMenuEvent(QContextMenuEvent *e)
{
	const QModelIndex i = indexAt(e->pos());
	if(i.isValid() && (i.flags() & Qt::ItemIsUserCheckable)) {

		QMenu m;
		m.addAction(tr("Select items"),[this](){
			activateItem(QompCon::DataSelect);
		});
		m.addAction(tr("Unselect items"),[this](){
			activateItem(QompCon::DataUnselect);
		});
		m.addAction(tr("Toggle items"),[this](){
			activateItem(QompCon::DataToggle);
		});

		m.exec(e->globalPos());
		e->accept();
		return;
	}

	QTreeView::contextMenuEvent(e);
}

void QompPluginTreeView::activateItem(QompCon::DataSelection action)
{
	for(const QModelIndex& i: selectionModel()->selectedIndexes()) {
		if(i.isValid() && (i.flags() & Qt::ItemIsUserCheckable)) {
			model()->setData(i, action, Qt::CheckStateRole);

			if(i.data(Qt::CheckStateRole) == Qt::Checked)
				emit itemActivated(i);
		}
	}
}

QRect QompPluginTreeView::branchIndicatorRectAt(const QModelIndex &index) const
{
	QStyleOptionViewItem opt = viewOptions();
	opt.rect = visualRect(index);
	QRect rect = style()->subElementRect(QStyle::SE_TreeViewDisclosureItem, &opt);
	return rect;
}
