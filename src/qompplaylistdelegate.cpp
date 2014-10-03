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

void QompPlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &o, const QModelIndex &index) const
{
	if(!index.isValid())
		return;

	painter->save();
	QRect rect = o.rect;
	QPalette palette = o.palette;
	QFont font = o.font;
	QBrush brush(palette.color(QPalette::Base));
	if(index.data(QompPlayListModel::IsCurrentTuneRole).toBool()) {
		QLinearGradient grad(rect.left(), 0, rect.right(), 0);
		grad.setColorAt(0, palette.color(QPalette::Base));
		grad.setColorAt(1, palette.color(QPalette::Base));
		grad.setColorAt(0.5, QColor("#F5F5F5"));
		brush = QBrush(grad);
		palette.setColor(QPalette::Text, "#4C8DC7");
		font.setWeight(QFont::Bold);
	}
	else {
		palette.setColor(QPalette::Text, "#3244C9");
	}
	QString text;
	QString title = index.data(QompPlayListModel::TitleRole).toString();
	if(!title.isEmpty()) {
		text = QString::number(index.row()+1) + ". ";
		QString artist = index.data(QompPlayListModel::ArtistRole).toString();
		if(!artist.isEmpty())
			text += artist + " - ";
		text += title;
	}
	if(text.isEmpty()) {
		text = index.data(QompPlayListModel::URLRole).toString();
	}
	if(text.isEmpty()) {
		text = index.data(QompPlayListModel::FileRole).toString();
		text = QFileInfo(text).fileName();
	}

	painter->fillRect(rect, (o.state & QStyle::State_Selected) ? palette.color(QPalette::Highlight) : brush);

	painter->setPen(QPen((o.state & QStyle::State_Selected) ? palette.color(QPalette::HighlightedText) : palette.color(QPalette::Text)));
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
