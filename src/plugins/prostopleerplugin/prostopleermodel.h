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

#ifndef PROSTOPLEERMODEL_H
#define PROSTOPLEERMODEL_H

#include <QAbstractListModel>
#include <QSet>

struct ProstopleerTune
{
	QString title;
	QString artist;
	QString duration;
	QString id;
	QString url;

	QString durationToStr() const;
	bool operator==(const ProstopleerTune& other);
};

class ProstopleerModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit ProstopleerModel(QObject *parent = 0);
	void addTunes(const QList<ProstopleerTune>& tunes);
	ProstopleerTune tune(const QModelIndex& index) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	void urlChanged(const QString& id, const QString& url);
	void reset();

private:
	QList<ProstopleerTune> tunes_;
	QSet<QModelIndex> selected_;
};

#endif // PROSTOPLEERMODEL_H
