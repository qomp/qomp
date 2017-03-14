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

#include "qompplaylistview.h"
#include "qompplaylistmodel.h"
#include "tune.h"

#include <QDropEvent>
#include <QBuffer>
#include <QToolTip>

QompPlaylistView::QompPlaylistView(QWidget *parent) :
	QListView(parent)
{
	setDragEnabled(true);
	setSelectionMode(ExtendedSelection);
	setDragDropMode(DragDrop);
	setDefaultDropAction(Qt::MoveAction);
}

void QompPlaylistView::startDrag(Qt::DropActions supportedActions)
{
	selected_.clear();
	foreach(const QModelIndex& ind, selectionModel()->selectedIndexes()) {
		selected_.append( static_cast<QompPlayListModel*>(model())->tune(ind) );
	}
	QListView::startDrag(supportedActions);
}

void QompPlaylistView::dropEvent(QDropEvent *e)
{
	QListView::dropEvent(e);

	if(e->dropAction() == Qt::MoveAction) {
		QItemSelection sel;
		foreach(Tune* t, selected_) {
			sel.append(QItemSelectionRange(static_cast<QompPlayListModel*>(model())->indexForTune(t)));
		}
		if(!sel.isEmpty()) {
			selectionModel()->select(sel,QItemSelectionModel::ClearAndSelect);
		}
		selected_.clear();
	}
}

bool QompPlaylistView::viewportEvent(QEvent *e)
{
	if(e->type() == QEvent::ToolTip) {
		QHelpEvent *he = static_cast<QHelpEvent*>(e);
		const QModelIndex ind = indexAt(he->pos());

		if(ind.isValid()) {
			QString html;
			const QImage img = ind.data(QompPlayListModel::CoverRole).value<QImage>();
			if(!img.isNull()) {
				QByteArray data;
				QBuffer buffer(&data);
				img.scaled(64, 64, Qt::KeepAspectRatio,
					   Qt::SmoothTransformation).save(&buffer, "PNG", 100);
				html = QString("<table><tr>"
					       "<td><img src='data:image/png;base64, %1'></td>"
					       "<td>&nbsp;&nbsp;</td>"
					       "<td>%2</td>"
					       "</tr></table>")
						.arg(QString(data.toBase64()))
						.arg(ind.data(Qt::ToolTipRole).toString());
			}
			else
				html = ind.data(Qt::ToolTipRole).toString();

			QToolTip::showText(he->globalPos(), html, this, QRect(), 5000);
		}
		else {
			QToolTip::hideText();
			e->ignore();
		}

		return true;
	}
	return QListView::viewportEvent(e);
}

QSize QompPlaylistView::minimumSizeHint() const
{
	QSize sz = QListView::minimumSizeHint();
	sz.setHeight(0);

	return sz;
}

QSize QompPlaylistView::sizeHint() const
{
	QSize sz = QListView::sizeHint();
	sz.setHeight(0);

	return sz;
}
