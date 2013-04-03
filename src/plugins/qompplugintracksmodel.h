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

#ifndef QOMPPLUGINTRACKSMODEL_H
#define QOMPPLUGINTRACKSMODEL_H
	
#include <QAbstractListModel>
#include <QSet>
#include <qompplugintypes.h>

class QompPluginModelItem;

class QompPluginTracksModel : public QAbstractListModel
{
	Q_OBJECT
public:
	QompPluginTracksModel(QObject *parent = 0);
	~QompPluginTracksModel();

	void addTunes(const QList<QompPluginModelItem*> &tunes);
	QompPluginModelItem* tune(const QModelIndex& index) const;
	QompPluginModelItem* tuneForId(const QString& id) const;
	QList<QompPluginModelItem*> selectedTunes() const;
	virtual void reset();

	void emitUpdateSignal();

	//reimlemented from QAbstractListModel
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:
	QSet<QModelIndex> selected_;
	QList<QompPluginModelItem*> tunes_;
};


#endif // QOMPPLUGINTRACKSMODEL_H
