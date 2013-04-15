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

#include "qompplaylistmodel.h"
#include "common.h"

#include <QFileInfo>
#include <QStringList>

QompPlayListModel::QompPlayListModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

void QompPlayListModel::addTunes(const TuneList &tunes)
{
	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size());
	tunes_.append(tunes);
	emit endInsertRows();
}

Tune QompPlayListModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return Tune();

	return tunes_.at(index.row());
}

Tune QompPlayListModel::currentTune() const
{
	return currentTune_;
}

void QompPlayListModel::setCurrentTune(const Tune &tune)
{
	currentTune_ = tune;
}

void QompPlayListModel::removeTune(const Tune &tune)
{
	for(int i = 0; i < tunes_.size(); i++) {
		if(tunes_.at(i) == tune) {
			beginRemoveRows(QModelIndex(), i, i);
			tunes_.removeAt(i);
			endRemoveRows();
			return;
		}
	}
}

QModelIndex QompPlayListModel::indexForTune(const Tune &tune) const
{
	for(int i = 0; i < tunes_.size(); i++) {
		if(tunes_.at(i) == tune) {
			return index(i,0);
		}
	}
	return QModelIndex();
}

QVariant QompPlayListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return QVariant();

	if(role == Qt::DisplayRole && index.column() == 0) {
		Tune t = tunes_.at(index.row());
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
		return tunes_.at(index.row()).artist;
	}
	else if(role == TitleRole) {
		Tune t = tunes_.at(index.row());
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
		return tunes_.at(index.row()).trackNumber;
	}
	else if(role == Qt::ToolTipRole) {
		QString ret;
		Tune t = tunes_.at(index.row());
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

	else if(role == DurationRole) {
		return tunes_.at(index.row()).duration;
	}
	else if(role == FileRole) {
		return tunes_.at(index.row()).file;
	}
	else if(role == URLRole) {
		return tunes_.at(index.row()).url;
	}

	return QVariant();
}

int QompPlayListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return tunes_.size();
}

void QompPlayListModel::clear()
{
	beginResetModel();
	tunes_.clear();
	currentTune_ = Tune();
	endResetModel();
}

void QompPlayListModel::newDataReady(const Tune &tune, const QMap<QString, QString> &data)
{
	TuneList::iterator it = tunes_.begin();
	for(; it != tunes_.end(); ++it) {
		if((*it) == tune) {
			emit layoutAboutToBeChanged();
			(*it).artist = data.value("ARTIST");
			(*it).album = data.value("ALBUM");
			(*it).title = data.value("TITLE");
			(*it).trackNumber = data.value("TRACK-NUMBER");
			(*it).bitRate = data.value("BITRATE");
			emit layoutChanged();
			break;
		}
	}
}

void QompPlayListModel::totalTimeChanged(const Tune &tune, qint64 msec)
{
	TuneList::iterator it = tunes_.begin();
	for(; it != tunes_.end(); ++it) {
		if((*it) == tune) {
			emit layoutAboutToBeChanged();
			if(msec != -1 && msec != 0)
				(*it).duration = durationMiliSecondsToString(msec);
			emit layoutChanged();
			break;
		}
	}
}
