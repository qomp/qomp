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
#include "options.h"
#include "defines.h"

#include <QFileInfo>
#include <QStringList>
#include <QMimeData>
#include <QTextStream>

static const QString cachedPlayListFileName = "/qomp-cached-playlist.qomp";



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

Tune *QompPlayListModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return (Tune*)Tune::emptyTune();

	return tunes_.at(index.row());
}

Tune *QompPlayListModel::currentTune() const
{
	return currentTune_;
}

QModelIndex QompPlayListModel::currentIndex() const
{
	return indexForTune(currentTune());
}

void QompPlayListModel::setCurrentTune(Tune *tune)
{
	if(currentTune_ != tune) {
		currentTune_ = tune;
		emit currentTuneChanged(tune);
		emit layoutChanged();
	}
}

void QompPlayListModel::removeTune(Tune *tune)
{
	int i = tunes_.indexOf(tune);
	if(i != -1) {
		beginRemoveRows(QModelIndex(), i, i);
		tunes_.removeAt(i);
		delete tune;
		endRemoveRows();
	}
}

QModelIndex QompPlayListModel::indexForTune(Tune *tune) const
{
	int i = tunes_.indexOf(tune);
	if(i != -1) {
		return index(i,0);
	}

	return QModelIndex();
}

QVariant QompPlayListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size() || index.column() != 0)
		return QVariant();

	const Tune* t = tunes_.at(index.row());
	if(role == Qt::DisplayRole) {
		QString ret;
		if(!t->title.isEmpty()) {
			if(!t->trackNumber.isEmpty()) {
				ret = t->trackNumber+". ";
			}
			if(!t->artist.isEmpty()) {
				ret += t->artist+" - ";
			}
			ret += t->title;
			if(!t->duration.isEmpty()) {
				ret += QString("    [%1]").arg(t->duration);
			}
			return QString("%1.%2").arg(QString::number(index.row()+1), ret);
		}

		QString fn;
		if(t->file.isEmpty())
			fn = t->url;
		else {
			QFileInfo fi(t->file);
			fn = fi.baseName();
		}
		if(!t->duration.isEmpty()) {
			fn += QString("    [%1]").arg(t->duration);
		}
		return QString("%1.%2").arg(QString::number(index.row()+1), fn);
	}
	else if(role == ArtistRole) {
		return t->artist;
	}
	else if(role == TitleRole) {
		QString title = t->title;
		if(title.isEmpty()) {
			if(!t->file.isEmpty()) {
				QFileInfo fi(t->file);
				title = fi.baseName();
			}
		}
		return title;
	}
	else if(role == TrackRole) {
		return t->trackNumber;
	}
	else if(role == Qt::ToolTipRole) {
		QString ret;
		if(!t->artist.isEmpty()) {
			ret += tr("Artist: %1\n").arg(t->artist);
		}
		if(!t->title.isEmpty()) {
			ret += tr("Title: %1\n").arg(t->title);
		}
		if(!t->album.isEmpty()) {
			ret += tr("Album: %1\n").arg(t->album);
		}
		if(!t->bitRate.isEmpty()) {
			ret += tr("Bitrate: %1\n").arg(t->bitRate);
		}
		ret.chop(1);
		return ret;
	}

	else if(role == DurationRole) {
		return t->duration;
	}
	else if(role == FileRole) {
		return t->file;
	}
	else if(role == URLRole) {
		return t->url;
	}
	else if(role == IsCurrentTuneRole) {
		return currentTune() == t;
	}

	return QVariant();
}

int QompPlayListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return tunes_.size();
}

Qt::DropActions QompPlayListModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::ItemFlags QompPlayListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid())
		return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
	else
		return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList QompPlayListModel::mimeTypes() const
{
	QStringList list;
	list.append("qomp/tune");
	return list;
}

QMimeData *QompPlayListModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData();
	QByteArray encodedData;

	QDataStream stream(&encodedData, QIODevice::WriteOnly);

	foreach (const QModelIndex &index, indexes) {
		if (index.isValid()) {
			stream << index.row();
		}
	}
	mimeData->setData("qomp/tune", encodedData);
	return mimeData;
}

bool QompPlayListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!data->hasFormat("qomp/tune"))
		return false;

	if (column > 0)
		return false;


	if (row == -1) {
		if(parent.isValid()) {
			row = parent.row();
		}
		else {
			row = rowCount();
		}
	}

	Tune* t = tune(index(row));
	QByteArray encodedData = data->data("qomp/tune");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	TuneList tl;
	while (!stream.atEnd()) {
		int i;
		stream >> i;
		tl.append(tune(index(i)));
	}

	layoutAboutToBeChanged();
	foreach(Tune* t, tl) {
		tunes_.removeOne(t);
	}
	int tuneIndex = indexForTune(t).row();
	if(tuneIndex == -1)
		tuneIndex = rowCount()+1;
	foreach(Tune* t, tl) {
		tunes_.insert(tuneIndex++, t);
	}
	layoutChanged();
	return true;
}

void QompPlayListModel::clear()
{
	beginResetModel();
	qDeleteAll(tunes_);
	tunes_.clear();
	currentTune_ = (Tune*)Tune::emptyTune();
	endResetModel();
}

void QompPlayListModel::saveState()
{
	int curTrack = 0;
	QModelIndex ind = currentIndex();
	if(ind.isValid())
		curTrack = ind.row();
	Options::instance()->setOption(OPTION_CURRENT_TRACK, curTrack);
	saveTunes(Qomp::cacheDir() + cachedPlayListFileName);
}

void QompPlayListModel::restoreState()
{
	TuneList tl = Tune::tunesFromFile(Qomp::cacheDir() + cachedPlayListFileName);
	if(!tl.isEmpty()) {
		addTunes(tl);
		QModelIndex ind = index(Options::instance()->getOption(OPTION_CURRENT_TRACK, 0).toInt(),0);
		setCurrentTune(tune(ind));
		//ui->playList->setCurrentIndex(ind);
	}
}

void QompPlayListModel::saveTunes(const QString &fileName)
{
	QString f(fileName);
	if(!f.endsWith(".qomp"))
		f += ".qomp";

	QFile file(f);
	if(file.open(QFile::ReadWrite | QFile::Truncate)) {
		QTextStream ts(&file);
		ts.setCodec("UTF-8");
		foreach(Tune* t, tunes_) {
			ts << t->toString() << endl;
		}
	}
}

void QompPlayListModel::loadTunes(const QString &fileName)
{
	TuneList tl = Tune::tunesFromFile(fileName);
	addTunes(tl);
}

void QompPlayListModel::tuneDataUpdated(Tune *tune)
{
	int i = tunes_.indexOf(tune);
	emit dataChanged(index(i), index(i));
}
