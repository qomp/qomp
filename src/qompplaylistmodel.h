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

#ifndef QOMPPLAYLISTMODEL_H
#define QOMPPLAYLISTMODEL_H

#include <QAbstractListModel>

class Tune;

class QompPlayListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	QompPlayListModel(QObject *parent = 0);

	enum QompPlayListModelRole {
		ArtistRole = Qt::UserRole+1,
		TitleRole = Qt::UserRole+2,
		TrackRole = Qt::UserRole+3,
		DurationRole = Qt::UserRole+4,
		URLRole = Qt::UserRole+5,
		FileRole = Qt::UserRole+6,
		IsCurrentTuneRole = Qt::UserRole+7,
		CanDownloadRole = Qt::UserRole+8
	};

	void addTunes(const QList<Tune*>& tunes);
	Tune *tune(const QModelIndex& index) const;
	Tune* currentTune() const;
	QModelIndex currentIndex() const;
	void setCurrentTune(Tune *tune);
	void removeTune(Tune* tune);
	QModelIndex indexForTune(Tune* tune) const;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	Qt::DropActions supportedDropActions() const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data,
	     Qt::DropAction action, int row, int column, const QModelIndex &parent);

	void clear();
	void saveState();
	void restoreState();

	void saveTunes(const QString& fileName);
	void loadTunes(const QString& fileName);

	virtual QHash<int, QByteArray> roleNames() const;

signals:
	void currentTuneChanged(Tune*);
	void totalTimeChanged(uint);

public slots:
	void tuneDataUpdated(Tune* tune);
#ifdef QOMP_MOBILE
	void move(int oldRow, int newRow);
#endif

private:
	uint totalTime() const;
	void updateTuneTracks();

private:
	QList<Tune*> tunes_;
	Tune* currentTune_;
};

#endif // PLAYLISTMODEL_H
