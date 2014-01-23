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

#include "qompplugintypes.h"
#include "tune.h"
#include <QStyle>
#include <QApplication>

//-----------------------
//----QompPluginItem-----
//-----------------------
QompPluginModelItem::QompPluginModelItem(QompPluginModelItem *parent) :
	parent_(parent),
	model_(0)
{
}

QompPluginModelItem::~QompPluginModelItem()
{
	qDeleteAll(items_);
	items_.clear();
}

void QompPluginModelItem::setParent(QompPluginModelItem *parent)
{
	parent_ = parent;
}

QompPluginModelItem *QompPluginModelItem::parent() const
{
	return parent_;
}

void QompPluginModelItem::setItems(QList<QompPluginModelItem *> items)
{
	qDeleteAll(items_);
	items_.clear();
	foreach(QompPluginModelItem *it, items)
		it->setParent(this);
	items_ = items;
}

void QompPluginModelItem::addItems(QList<QompPluginModelItem *> items)
{
	foreach(QompPluginModelItem *it, items)
		it->setParent(this);
	items_.append(items);
}

QAbstractItemModel *QompPluginModelItem::model() const
{
	if(parent_)
		return parent_->model();

	return model_;
}

void QompPluginModelItem::setModel(QAbstractItemModel *model)
{
	if(parent_)
		parent_->setModel(model);
	else
		model_ = model;
}

QList<QompPluginModelItem *> QompPluginModelItem::items() const
{
	return items_;
}


//-----------------------
//----QompPluginTune-----
//-----------------------
QompPluginTune::QompPluginTune(QompPluginModelItem *parent) :
	QompPluginModelItem(parent)
{
}

QString QompPluginTune::toString() const
{
	QString ret = QString("%1 - %2").arg(artist, title);
//	if(!url.isNull())
//		ret += "  [OK]";
	return ret;
}

Qomp::ModelItemType QompPluginTune::type() const
{
	return Qomp::TypeTune;
}

QIcon QompPluginTune::icon() const
{
	static const QIcon ico(":/icons/icons/tune.png");
	return QIcon(ico.pixmap(QSize(64,64),url.isEmpty() ? QIcon::Disabled : QIcon::Normal));
}

Tune QompPluginTune::toTune() const
{
	Tune t;
	t.artist = artist;
	t.title = title;
	t.album = album;
	t.url = url;
	t.duration = duration;
	return t;
}


//-----------------------
//----QompPluginAlbum----
//-----------------------
QompPluginAlbum::QompPluginAlbum(QompPluginModelItem *parent) :
	QompPluginModelItem(parent),
	tunesReceived(false)
{
}

QString QompPluginAlbum::toString() const
{
	return	artist + " - " +
		album + " - " +
		year + " [" +
			QString::number(items().size()) + "]";
}

Qomp::ModelItemType QompPluginAlbum::type() const
{
	return Qomp::TypeAlbum;
}

QIcon QompPluginAlbum::icon() const
{
	return qApp->style()->standardIcon(QStyle::SP_DirIcon);
}



//-----------------------
//----QompPluginArtist---
//-----------------------
QompPluginArtist::QompPluginArtist(QompPluginModelItem *parent) :
	QompPluginModelItem(parent),
	tunesReceived(false)
{
}

QString QompPluginArtist::toString() const
{
	return	artist + "  [" + QString::number(items().size()) + "]";
}

Qomp::ModelItemType QompPluginArtist::type() const
{
	return Qomp::TypeArtist;
}

QIcon QompPluginArtist::icon() const
{
	return qApp->style()->standardIcon(QStyle::SP_DirIcon);
}
