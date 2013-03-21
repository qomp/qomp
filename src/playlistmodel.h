#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <Phonon/MediaSource>

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


private:
	typedef QPair <Phonon::MediaSource, Tune> Media;
	QList<Media> tunes_;
	Tune currentTune_;
};

#endif // PLAYLISTMODEL_H
