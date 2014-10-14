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

#include "qompplugintypes.h"
#include "tune.h"
#include <QPixmap>
#include <QPixmapCache>
#include <QIcon>

#include <algorithm>

static const
#ifdef Q_OS_ANDROID
	QString
#else
	QIcon
#endif
	&folderIcon()
{
#ifdef Q_OS_ANDROID
	static const QString ico("qrc:///icons/folder");
	return ico;
#else
	static QIcon ico;
	if(ico.isNull()) {
		ico = QIcon(":/icons/folder");
	}
	return ico;
#endif
}

static bool lessThen(QompPluginModelItem* it1, QompPluginModelItem* it2)
{
	if(it1->type() != it2->type())
		return it1->type() > it2->type();

	if(it1->type() == QompCon::TypeAlbum) {
		QompPluginAlbum* a1 = static_cast<QompPluginAlbum*>(it1);
		QompPluginAlbum* a2 = static_cast<QompPluginAlbum*>(it2);

		if(a1->parent() && a2->parent()) {
			QompPluginArtist* ar1 = static_cast<QompPluginArtist*>(a1->parent());
			QompPluginArtist* ar2 = static_cast<QompPluginArtist*>(a2->parent());

			if(a1->artist != ar1->artist && a2->artist == ar2->artist)
				return false;

			if(a1->artist == ar1->artist && a2->artist != ar2->artist)
				return true;
		}


		if(a1->year != a2->year)
			return a1->year.toInt() > a2->year.toInt();

		return a1->album < a2->album;
	}

	return it1->toString() < it2->toString();
}

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

	if(items_.size() > 1)
		sortChildren();
}

void QompPluginModelItem::addItems(QList<QompPluginModelItem *> items)
{
	foreach(QompPluginModelItem *it, items)
		it->setParent(this);
	items_.append(items);

	if(items_.size() > 1)
		sortChildren();
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

void QompPluginModelItem::sortChildren()
{
	std::sort(items_.begin(), items_.end(), lessThen);
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

QompCon::ModelItemType QompPluginTune::type() const
{
	return QompCon::TypeTune;
}

#ifdef Q_OS_ANDROID
QString
#else
QIcon
#endif
	QompPluginTune::icon() const
{
#ifdef Q_OS_ANDROID
	static const QString icoDis("qrc:///icons/tune_disabled");
	static const QString icoEn("qrc:///icons/tune");
	return url.isEmpty() ? icoDis : icoEn;
#else
	static QIcon icoDis;
	if(icoDis.isNull()) {
		icoDis = QIcon(":/icons/tune_disabled");
	}
	static QIcon icoEn;
	if(icoEn.isNull()) {
		icoEn = QIcon(":/icons/tune");
	}
	return url.isEmpty() ? icoDis : icoEn;
#endif
}

Tune *QompPluginTune::toTune() const
{
	Tune* t = new Tune;
	t->artist = artist;
	t->title = title;
	t->album = album;
	t->url = url;
	t->duration = duration;
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
	QString ret = "";
	if(!artist.isEmpty() && (!parent() || static_cast<QompPluginArtist*>(parent())->artist != artist))
		ret += artist + " - ";
	ret += album + " - " +
		year + " [" + QString::number(items().size()) + "]";
	return ret;
}

QompCon::ModelItemType QompPluginAlbum::type() const
{
	return QompCon::TypeAlbum;
}
#ifdef Q_OS_ANDROID
QString
#else
QIcon
#endif
QompPluginAlbum::icon() const
{
	return folderIcon();
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

QompCon::ModelItemType QompPluginArtist::type() const
{
	return QompCon::TypeArtist;
}

#ifdef Q_OS_ANDROID
QString
#else
QIcon
#endif
QompPluginArtist::icon() const
{
	return folderIcon();
}
