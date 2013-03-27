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

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <Phonon/MediaSource>
namespace Phonon {
	class MediaObject;
}

#include "tune.h"

class PlayListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	PlayListModel(QObject *parent = 0);

	enum PlayListModelRole {
		ArtistRole = Qt::UserRole+1,
		TitleRole = Qt::UserRole+2,
		TrackRole = Qt::UserRole+3
	};

	void addTunes(const TuneList& tunes);
	Tune tune(const QModelIndex& index) const;
	Tune currentTune() const;
	void setCurrentTune(const Tune& tune);
//	QIODevice* device(const Tune& tune) const;
	Phonon::MediaSource device(const QModelIndex& index) const;
	QModelIndex indexForTune(const Tune& tune) const;

	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	void clear();

private slots:
	void metaDataReady();
	void resolverStateChanged(Phonon::State newState, Phonon::State oldState);
	void totalTimeChanged(qint64 msec);

private:
	const Phonon::MediaSource mediaSourceForId(int id) const;

private:
	struct Media {
		Media(const Phonon::MediaSource& ms, const Tune& t) :
			mediaSource(ms),
			tune(t),
			id(t.id())
		{}

		Phonon::MediaSource mediaSource;
		Tune tune;
		int id;
	};
	QList<Media> tunes_;
	Tune currentTune_;
	Phonon::MediaObject* resolver_;
	QList<int> tuneIdsForResolve_;
};

#endif // PLAYLISTMODEL_H
