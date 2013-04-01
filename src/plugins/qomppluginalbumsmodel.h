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

#ifndef QOMPPLUGINALBUMSMODEL_H
#define QOMPPLUGINALBUMSMODEL_H

#include <QAbstractItemModel>
#include <QSet>

class QompPluginAlbum;
class QompPluginTune;

class QompPluginAlbumsModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	QompPluginAlbumsModel(QObject *parent = 0);
	~QompPluginAlbumsModel();

	void addAlbums(const QList<QompPluginAlbum*>& albums);
	void setTunes(const QList<QompPluginTune*>& tunes, const QString& parentAlbumId);

	bool isAlbum(const QModelIndex& index) const;
	QompPluginAlbum* album(const QModelIndex& index) const;
	QompPluginAlbum* album(const QString& id) const;
	QModelIndex indexForAlbum(QompPluginAlbum* album) const;

	QompPluginTune* tune(const QModelIndex& index) const;
	QList<QompPluginTune*> selectedTunes() const;
	virtual bool urlChanged(const QString& id, const QString& url);

	void validateSelection(const QModelIndex& parent);
	virtual void reset();

	//reimplemented from QAbstractItemModel
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

protected:
	QList<QompPluginAlbum*> albums_;

private:
	QSet<QModelIndex> selected_;
};
#endif // QOMPPLUGINALBUMSMODEL_H
