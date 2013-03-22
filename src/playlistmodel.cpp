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

#include "playlistmodel.h"
#include <QFileInfo>
#include <QStringList>

PlayListModel::PlayListModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

void PlayListModel::addTunes(const TuneList &tunes)
{

	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size());
	foreach(const Tune& tune, tunes) {
		tunes_.append(Media(Phonon::MediaSource(), tune));
	}
	emit endInsertRows();
}

Tune PlayListModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return Tune();

	return tunes_.at(index.row()).second;
}

Tune PlayListModel::currentTune() const
{
	return currentTune_;
}

void PlayListModel::setCurrentTune(const Tune &tune)
{
	currentTune_ = tune;
}

//QIODevice *PlayListModel::device(const Tune& tune) const
//{
//	foreach(const media& pair, tunes_) {
//		if((Tune)pair.second == tune)
//			return pair.first;
//	}
//	return 0;
//}

Phonon::MediaSource PlayListModel::device(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		Phonon::MediaSource();

	const Media &media = tunes_.at(index.row());
	Phonon::MediaSource &ms = (Phonon::MediaSource&)media.first;
	if(ms.type() == Phonon::MediaSource::Invalid || ms.type() == Phonon::MediaSource::Empty) {
		Tune tune = tunes_.at(index.row()).second;
		if(!tune.file.isEmpty()) {
			ms = Phonon::MediaSource(tune.file);
		}
		else if(!tune.url.isEmpty()) {
			ms = Phonon::MediaSource(tune.url);
		}
	}
	return ms;
}

QModelIndex PlayListModel::indexForTune(const Tune& tune) const
{
	for(int i = 0; i < tunes_.size(); i++) {
		if((Tune)tunes_.at(i).second == tune) {
			return index(i,0);
		}
	}
	return QModelIndex();
}

QVariant PlayListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return QVariant();

	if(role == Qt::DisplayRole && index.column() == 0) {
		Tune t = tunes_.at(index.row()).second;
		QString ret;
		QStringList fields;
		fields << t.trackNumber << t.artist << t.title;
		ret = fields.join(" ").trimmed();
		if(!ret.isEmpty()) {
			return QString("%1.%2").arg(QString::number(index.row()+1), ret);
		}

		QString fn;
		if(t.file.isEmpty())
			fn = t.url;
		else {
			QFileInfo fi(t.file);
			fn = fi.baseName();
		}
		return QString("%1.%2").arg(QString::number(index.row()+1), fn);
	}
	if(role == ArtistRole) {
		return tunes_.at(index.row()).second.artist;
	}
	if(role == TitleRole) {
		Tune t = tunes_.at(index.row()).second;
		QString title = t.title;
		if(title.isEmpty()) {
			if(!t.file.isEmpty()) {
				QFileInfo fi(t.file);
				title = fi.baseName();
			}
		}
		return title;
	}
	if(role == TrackRole) {
		return tunes_.at(index.row()).second.trackNumber;
	}

	return QVariant();
}

int PlayListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return tunes_.size();
}

void PlayListModel::clear()
{
	beginResetModel();
	tunes_.clear();
	currentTune_ = Tune();
	endResetModel();
}
