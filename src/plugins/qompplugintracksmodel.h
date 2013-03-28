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

struct QompPluginTune
{
	QString title;
	QString artist;
	QString duration;
	QString id;
	QString url;
};

class QompPluginTracksModel : public QAbstractListModel
{
	Q_OBJECT
public:
	QompPluginTracksModel(QObject *parent = 0);
	void addTunes(const QList<QompPluginTune> &tunes);
	QompPluginTune tune(const QModelIndex& index) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	enum { DataSelect = 1, DataUnselect = 0, DataToggle = 2 };
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual void urlChanged(const QString& id, const QString& url);
	virtual void reset();

protected:
	QList<QompPluginTune> tunes_;

private:
	QSet<QModelIndex> selected_;
};


#endif // QOMPPLUGINTRACKSMODEL_H
