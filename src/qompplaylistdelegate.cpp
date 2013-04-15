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

QompPlaylistDelegate::QompPlaylistDelegate(QObject *parent) :
	QItemDelegate(parent)
{
}

void QompPlaylistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &o, const QModelIndex &index) const
{
	if(!index.isValid())
		return;

	painter->save();
	QFontMetrics fm = o.fontMetrics;
	QRect rect = o.rect;
	QPalette palette = o.palette;
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

	painter->fillRect(rect, (o.state & QStyle::State_Selected) ? palette.color(QPalette::Highlight) : palette.color(QPalette::Base));

	painter->setPen(QPen((o.state & QStyle::State_Selected) ? palette.color(QPalette::HighlightedText) : palette.color(QPalette::Text)));
	painter->setFont(o.font);

	QRect durRect(rect);
	QString dur = index.data(QompPlayListModel::DurationRole).toString();
	int w = fm.width(dur);
	durRect.setLeft(rect.right() - w - 1);
	rect.setRight(durRect.left() - 1);
	text = fm.elidedText(text, Qt::ElideRight,rect.width());

	painter->drawText(rect, text);
	painter->drawText(durRect, dur);

	painter->restore();
}

QSize QompPlaylistDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
	return QSize(0, option.fontMetrics.height());
}
