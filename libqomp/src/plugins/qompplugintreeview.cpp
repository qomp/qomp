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
#include <QTimer>


QompPluginTreeView::QompPluginTreeView(QWidget *parent) :
	QTreeView(parent)
{
	setHeaderHidden(true);
	setItemsExpandable(true);
	setUniformRowHeights(true);
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

void QompPluginTreeView::mousePressEvent(QMouseEvent *e)
{
	QModelIndex index = indexAt(e->pos());

	if(index.isValid()&& e->modifiers() == Qt::ShiftModifier
		&& (index.flags() & Qt::ItemIsUserCheckable))
	{
		QRect rect = checkBoxRectAt(index);

		if (rect.contains(e->pos())) {
			QModelIndex old = currentIndex();
			QModelIndex par = index.parent();

			if(old.isValid() && old.parent() == par) {
				int start = qMin(old.row(), index.row());
				int end = qMax(old.row(), index.row());
				int col = index.column();

				indexes_.clear();
				for(int i = start; i <= end; ++i) {
					indexes_.append(model()->index(i, col, par));
				}

				e->accept();
				setCurrentIndex(index);
				QTimer::singleShot(100, this, SLOT(updateIndexes()));
				return;
			}

		}
	}

	QTreeView::mousePressEvent(e);
	setCurrentIndex(index);
}

void QompPluginTreeView::mouseMoveEvent(QMouseEvent *e)
{
	//without this QTreeView accepts the event so it is not propogadet to paren widget
	if(e->button() == Qt::NoButton)
		e->ignore();
	else
		QTreeView::mouseMoveEvent(e);
}

void QompPluginTreeView::updateIndexes()
{
	foreach (const QModelIndex& ind, indexes_) {
		model()->setData(ind, QompCon::DataSelect, Qt::CheckStateRole);
		emit clicked(ind);
	}
}

void QompPluginTreeView::itemActivated()
{
	QModelIndex i = currentIndex();
	if(i.isValid()) {
		model()->setData(i, QompCon::DataToggle, Qt::CheckStateRole);
	}
}

QRect QompPluginTreeView::checkBoxRectAt(const QModelIndex &index) const
{
	QStyleOptionButton opt;
	opt.QStyleOption::operator=(viewOptions());
	opt.rect = visualRect(index);
	QRect rect = style()->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt);
	return rect;
}

