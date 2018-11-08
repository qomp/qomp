/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#include "qompplaylistdelegate.h"
#include "qompplaylistmodel.h"

#include <QPainter>
#include <QFileInfo>
#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

QompPlaylistDelegate::QompPlaylistDelegate(QObject *parent) :
	QItemDelegate(parent)
{
}

void QompPlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.isValid())
		return;

	painter->save();
	QRect rect = option.rect;
	QPalette palette = option.palette;
	QFont font = option.font;
	QBrush brush(option.palette.color(QPalette::Base));
	if(index.data(QompPlayListModel::IsCurrentTuneRole).toBool()) {
		QLinearGradient grad(rect.left(), 0, rect.right(), 0);
		grad.setColorAt(0, palette.color(QPalette::Base));
		grad.setColorAt(1, palette.color(QPalette::Base));
		grad.setColorAt(0.5, palette.color(QPalette::Base).lighter(110));
		brush = QBrush(grad);
		palette.setColor(QPalette::Text, palette.color(QPalette::Text).darker(110));
		font.setWeight(QFont::Bold);
	}

	QString text = QString::number(index.row()+1) + ". ";
	QString title = index.data(QompPlayListModel::TitleRole).toString();
	if(!title.isEmpty()) {
		QString artist = index.data(QompPlayListModel::ArtistRole).toString();
		if(!artist.isEmpty())
			text += artist + " - ";
		text += title;
	}
	else {
		QString str = index.data(QompPlayListModel::URLRole).toString();
		if(str.isEmpty()) {
			str = index.data(QompPlayListModel::FileRole).toString();
			if(!str.isEmpty()) {
				str = QFileInfo(str).fileName();
			}
		}

		if(!str.isEmpty()) {
			text += str;
		}
	}

	painter->fillRect(rect, (option.state & QStyle::State_Selected) ?
				  palette.color(QPalette::Highlight) : brush);

	painter->setPen(QPen((option.state & QStyle::State_Selected) ?
				     palette.color(QPalette::HighlightedText) : palette.color(QPalette::Text)));
	painter->setFont(font);

	static const int margine = 5;
	QRect durRect(rect);
	QString dur = index.data(QompPlayListModel::DurationRole).toString();
	int w = painter->fontMetrics().width(dur);
	durRect.setWidth(w + 1);
	durRect.moveRight(rect.right() - margine);
	rect.setRight(durRect.left() - 2);
	rect.setLeft(rect.left() + margine);
	text = painter->fontMetrics().elidedText(text, Qt::ElideRight,rect.width());

	painter->drawText(rect, text);
	painter->drawText(durRect, dur);

//	if(index.data(QompPlayListModel::IsCurrentTuneRole).toBool()) {
//		rect = o.rect;
//		rect.setRight(rect.right()-1);
//		rect.setBottom(rect.bottom()-1);
//		QPen p = painter->pen();
//		p.setStyle(Qt::DashLine);
//		painter->setPen(p);
//		painter->drawRect(rect);
//	}

	painter->restore();
}

QSize QompPlaylistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
	return QSize(0, option.fontMetrics.height());
}
