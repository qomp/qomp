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
#include "common.h"

#include <QFileInfo>
#include <QStringList>
#include <Phonon/MediaObject>

PlayListModel::PlayListModel(QObject *parent) :
	QAbstractListModel(parent)
{
	resolver_ = new Phonon::MediaObject(this);
	connect(resolver_, SIGNAL(metaDataChanged()), SLOT(metaDataReady()));
	connect(resolver_, SIGNAL(stateChanged(Phonon::State,Phonon::State)), SLOT(resolverStateChanged(Phonon::State,Phonon::State)));
	connect(resolver_, SIGNAL(totalTimeChanged(qint64)), SLOT(totalTimeChanged(qint64)));
}

void PlayListModel::addTunes(const TuneList &tunes)
{

	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size());
	foreach(const Tune& tune, tunes) {
		Phonon::MediaSource ms;
		if(!tune.file.isEmpty()) {
			ms = Phonon::MediaSource(tune.file);
			tuneIdsForResolve_.append(tune.id());
		}
		else if(!tune.url.isEmpty()) {
			ms = Phonon::MediaSource(tune.url);
		}
		tunes_.append(Media(ms, tune));
	}
	emit endInsertRows();

	if(!tuneIdsForResolve_.isEmpty()) {
		Phonon::MediaSource ms = mediaSourceForId(tuneIdsForResolve_.takeFirst());
		resolver_->setCurrentSource(ms);
		resolver_->pause();
	}
}

Tune PlayListModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return Tune();

	return tunes_.at(index.row()).tune;
}

Tune PlayListModel::currentTune() const
{
	return currentTune_;
}

void PlayListModel::setCurrentTune(const Tune &tune)
{
	currentTune_ = tune;
}

void PlayListModel::removeTune(const Tune &tune)
{
	for(int i = 0; i < tunes_.size(); i++) {
		if(tunes_.at(i).id == tune.id()) {
			beginRemoveRows(QModelIndex(), i, i);
			tunes_.removeAt(i);
			endRemoveRows();
			return;
		}
	}
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

	return tunes_.at(index.row()).mediaSource;
}

QModelIndex PlayListModel::indexForTune(const Tune& tune) const
{
	for(int i = 0; i < tunes_.size(); i++) {
		if((Tune)tunes_.at(i).tune == tune) {
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
		Tune t = tunes_.at(index.row()).tune;
		QString ret;
		if(!t.title.isEmpty()) {
			if(!t.trackNumber.isEmpty()) {
				ret = t.trackNumber+". ";
			}
			if(!t.artist.isEmpty()) {
				ret += t.artist+" - ";
			}
			ret += t.title;
			if(!t.duration.isEmpty()) {
				ret += QString("    [%1]").arg(t.duration);
			}
			return QString("%1.%2").arg(QString::number(index.row()+1), ret);
		}

		QString fn;
		if(t.file.isEmpty())
			fn = t.url;
		else {
			QFileInfo fi(t.file);
			fn = fi.baseName();
		}
		if(!t.duration.isEmpty()) {
			fn += QString("    [%1]").arg(t.duration);
		}
		return QString("%1.%2").arg(QString::number(index.row()+1), fn);
	}
	else if(role == ArtistRole) {
		return tunes_.at(index.row()).tune.artist;
	}
	else if(role == TitleRole) {
		Tune t = tunes_.at(index.row()).tune;
		QString title = t.title;
		if(title.isEmpty()) {
			if(!t.file.isEmpty()) {
				QFileInfo fi(t.file);
				title = fi.baseName();
			}
		}
		return title;
	}
	else if(role == TrackRole) {
		return tunes_.at(index.row()).tune.trackNumber;
	}
	else if(role == Qt::ToolTipRole) {
		QString ret;
		Tune t = tunes_.at(index.row()).tune;
		if(!t.artist.isEmpty()) {
			ret += tr("Artist: %1\n").arg(t.artist);
		}
		if(!t.title.isEmpty()) {
			ret += tr("Title: %1\n").arg(t.title);
		}
		if(!t.album.isEmpty()) {
			ret += tr("Album: %1\n").arg(t.album);
		}
		if(!t.bitRate.isEmpty()) {
			ret += tr("Bitrate: %1\n").arg(t.bitRate);
		}
		ret.chop(1);
		return ret;
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
	tuneIdsForResolve_.clear();
	endResetModel();
}

void PlayListModel::metaDataReady()
{
	Phonon::MediaSource ms = resolver_->currentSource();
	QList<Media>::iterator it = tunes_.begin();
	for(; it != tunes_.end(); ++it) {
		if((*it).mediaSource == ms) {
			emit layoutAboutToBeChanged();
			QMap<QString, QString> metaData = resolver_->metaData();
			(*it).tune.artist = metaData.value("ARTIST");
			(*it).tune.album = metaData.value("ALBUM");
			(*it).tune.title = metaData.value("TITLE");
			(*it).tune.trackNumber = metaData.value("TRACK-NUMBER");
			(*it).tune.bitRate = metaData.value("BITRATE");
			emit layoutChanged();
			break;
		}
	}
}

void PlayListModel::resolverStateChanged(Phonon::State newState, Phonon::State /*oldState*/)
{
	if(newState != Phonon::PausedState)
		return;

	if(!tuneIdsForResolve_.isEmpty()) {
		resolver_->setCurrentSource(mediaSourceForId(tuneIdsForResolve_.takeFirst()));
		resolver_->pause();
	}
	else
		resolver_->clear();
}

void PlayListModel::totalTimeChanged(qint64 msec)
{
	Phonon::MediaSource ms = resolver_->currentSource();
	QList<Media>::iterator it = tunes_.begin();
	for(; it != tunes_.end(); ++it) {
		if((*it).mediaSource == ms) {
			emit layoutAboutToBeChanged();
			if(msec != -1)
				(*it).tune.duration = durationMiliSecondsToString(msec);
			emit layoutChanged();
			break;
		}
	}
}

const Phonon::MediaSource PlayListModel::mediaSourceForId(int id) const
{
	foreach(const Media& m, tunes_) {
		if(m.id == id) {
			return m.mediaSource;
		}
	}
	return Phonon::MediaSource();
}
