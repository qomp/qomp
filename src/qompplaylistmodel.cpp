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
#include "tune.h"
#include "options.h"
#include "defines.h"
#include "common.h"
#include "pluginmanager.h"
#include "playlistparser.h"

#include <QFileInfo>
#include <QStringList>
#include <QMimeData>
#include <QTextStream>
#include <QDataStream>


static const QString cachedPlayListFileName = "/qomp-cached-playlist.qomp";
static const QString uriListMimeType = "text/uri-list";



QompPlayListModel::QompPlayListModel(QObject *parent) :
	QAbstractListModel(parent),
	currentTune_(const_cast<Tune*>(Tune::emptyTune()))
{
}

void QompPlayListModel::addTunes(const QList<Tune*> &tunes)
{
	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size()-1);
	tunes_.append(tunes);
	emit endInsertRows();
	updateTuneTracks();
	emit totalTimeChanged(totalTime());
}

Tune *QompPlayListModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return const_cast<Tune*>(Tune::emptyTune());

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
		emit layoutAboutToBeChanged();
		emit layoutChanged();
	}
}

void QompPlayListModel::removeTune(Tune *tune)
{
	if(tune == currentTune()) {
		currentTune_ = const_cast<Tune*>(Tune::emptyTune());
		emit currentTuneChanged(currentTune_);
	}

	int i = tunes_.indexOf(tune);
	if(i != -1) {
		beginRemoveRows(QModelIndex(), i, i);
		tunes_.removeAt(i);
		delete tune;
		endRemoveRows();
		updateTuneTracks();
		emit totalTimeChanged(totalTime());
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

bool QompPlayListModel::allTunesPlayed() const
{
	for(int i =0; i < rowCount(); ++i) {
		if(!tune(index(i))->played)
			return false;
	}

	return true;
}

void QompPlayListModel::unsetAllTunesPlayed()
{
	for(int i =0; i < rowCount(); ++i) {
		tune(index(i))->played = false;
	}
}

QVariant QompPlayListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size() || index.column() != 0)
		return QVariant();

	Tune* t = tunes_.at(index.row());
	if(role == Qt::DisplayRole) {
		return  t->displayString();
	}
	else if(role == ArtistRole) {
		return t->artist;
	}
	else if(role == AlbumRole) {
		return t->album;
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
	else if(role == BitrateRole) {
		return t->bitRate;
	}
	else if(role == Qt::ToolTipRole) {
		QString ret;
		if(!t->artist.isEmpty()) {
			ret += tr("<div><nobr><b>Artist</b>: %1</nobr></div>").arg(t->artist);
		}
		if(!t->title.isEmpty()) {
			ret += tr("<div><nobr><b>Title</b>: %1</nobr></div>").arg(t->title);
		}
		if(!t->album.isEmpty()) {
			ret += tr("<div><nobr><b>Album</b>: %1</nobr></div>").arg(t->album);
		}
		if(!t->bitRate.isEmpty()) {
			ret += tr("<div><nobr><b>Bitrate</b>: %1</nobr></div>").arg(t->bitRate);
		}
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
	else if(role == CanDownloadRole) {
		return t->canSave();
	}
	else if(role == TuneRole) {
		return QVariant::fromValue<Tune*>(t);
	}
	else if(role == CoverRole) {
		return t->cover();
	}

	return QVariant();
}

int QompPlayListModel::rowCount(const QModelIndex &/*parent*/) const
{
	return tunes_.size();
}

Qt::DropActions QompPlayListModel::supportedDropActions() const
{
	return Qt::MoveAction | Qt::CopyAction;
}

Qt::ItemFlags QompPlayListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

	if (index.isValid())
		return Qt::ItemIsDragEnabled | defaultFlags;
	else
		return Qt::ItemIsDropEnabled | defaultFlags;
}

QStringList QompPlayListModel::mimeTypes() const
{
	QStringList list;
	list << Tune::mimeDataName() << uriListMimeType;
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
	mimeData->setData(Tune::mimeDataName(), encodedData);
	return mimeData;
}

bool QompPlayListModel::canDropMimeData(const QMimeData *data, Qt::DropAction action,
					int /*row*/, int column, const QModelIndex &/*parent*/) const
{
	if (!data->hasFormat(Tune::mimeDataName()) && !data->hasFormat(uriListMimeType))
		return false;

	if(column > 0)
		return false;

	if(action != Qt::MoveAction && action != Qt::CopyAction)
		return false;

	return true;
}

bool QompPlayListModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	if(!canDropMimeData(data, action, row, column, parent))
		return false;

	if (action == Qt::IgnoreAction)
		return true;

	if (row == -1) {
		if(parent.isValid()) {
			row = parent.row();
		}
		else {
			row = rowCount();
		}
	}

	if (data->hasFormat(Tune::mimeDataName()) && action == Qt::MoveAction) {
		processTunesDrop(row, data);
		return true;
	}
	else if (data->hasFormat(uriListMimeType) && action == Qt::CopyAction) {
		processUrilistDrop(row, data);
		return true;
	}

	return false;
}

void QompPlayListModel::processTunesDrop(int row, const QMimeData *data)
{
	Tune* t = tune(index(row));
	QByteArray encodedData = data->data(Tune::mimeDataName());
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QList<Tune*> tl;
	while (!stream.atEnd()) {
		int i;
		stream >> i;
		tl.append(tune(index(i)));
	}

	emit layoutAboutToBeChanged();
	foreach(Tune* t, tl) {
		tunes_.removeOne(t);
	}
	int tuneIndex = indexForTune(t).row();
	if(tuneIndex == -1)
		tuneIndex = rowCount()+1;
	foreach(Tune* t, tl) {
		tunes_.insert(tuneIndex++, t);
	}
	emit layoutChanged();
	updateTuneTracks();
}

void QompPlayListModel::processUrilistDrop(int row, const QMimeData *data)
{
	QList<Tune*> tl;
	for(const QUrl url: data->urls()) {
		PluginManager::instance()->processUrl(url.toLocalFile(), &tl);
	}

	emit layoutAboutToBeChanged();

	for (Tune* t: tl) {
		tunes_.insert(row++,t);
	}

	emit layoutChanged();
	updateTuneTracks();
}

void QompPlayListModel::clear()
{
	beginResetModel();
	currentTune_ = const_cast<Tune*>(Tune::emptyTune());
	emit currentTuneChanged(currentTune_);
	qDeleteAll(tunes_);
	tunes_.clear();
	endResetModel();
	emit totalTimeChanged(totalTime());
}

void QompPlayListModel::saveState()
{
	int curTrack = 0;
	QModelIndex ind = currentIndex();
	if(ind.isValid())
		curTrack = ind.row();
	Options::instance()->setOption(OPTION_CURRENT_TRACK, curTrack);
	QString path = Qomp::cacheDir() + cachedPlayListFileName;
	saveTunes(path);
}

void QompPlayListModel::restoreState()
{
	QString path = Qomp::cacheDir() + cachedPlayListFileName;
	QList<Tune*> tl = Tune::tunesFromFile(path);
	if(!tl.isEmpty()) {
		addTunes(tl);
		QModelIndex ind = index(Options::instance()->getOption(OPTION_CURRENT_TRACK, 0).toInt(),0);
		setCurrentTune(tune(ind));
	}
}

void QompPlayListModel::saveTunes(const QString &fileName)
{
	if(fileName.isEmpty())
		return;

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
	if(fileName.isEmpty())
		return;

	QList<Tune*> tl;
	PlaylistParser p(fileName);
	if(p.canParse())
		tl = p.parse();
	else
		PluginManager::instance()->processUrl(fileName, &tl);

	if(!tl.isEmpty())
		addTunes(tl);
}

QHash<int, QByteArray> QompPlayListModel::roleNames() const
{
	static const QHash<int, QByteArray> roles{
		{Qt::DisplayRole,"text"},
		{Qt::ToolTipRole,"tooltip"},
		{ArtistRole,"artist"},
		{AlbumRole, "album"},
		{TrackRole,"track"},
		{BitrateRole,"bitrate"},
		{TitleRole, "title"},
		{DurationRole,"duration"},
		{URLRole,"url"},
		{FileRole,"file"},
		{IsCurrentTuneRole,"current"},
		{CanDownloadRole,"canDownload"},
		{CoverRole,"cover"}
	};
	return roles;
}

void QompPlayListModel::tuneDataUpdated(Tune *tune)
{
	int i = tunes_.indexOf(tune);
	emit dataChanged(index(i), index(i));
	emit totalTimeChanged(totalTime());
}

void QompPlayListModel::currentTuneTotalTimeChanged(qint64 ms)
{
	if(ms == -1 || ms == 0)
		return;

	currentTune()->duration = Qomp::durationMiliSecondsToString(ms);
	tuneDataUpdated(currentTune());
}
#ifdef QOMP_MOBILE
void QompPlayListModel::move(int oldRow, int newRow)
{
	Q_ASSERT(oldRow != newRow);
	Q_ASSERT(0 <= oldRow && oldRow < tunes_.size());
	Q_ASSERT(0 <= newRow && newRow < tunes_.size());

	int tmpRow = newRow;
	if (oldRow < newRow)
		++tmpRow;
	if( beginMoveRows(QModelIndex(), oldRow, oldRow, QModelIndex(), tmpRow) ) {
		tunes_.swap(oldRow, newRow);
		endMoveRows();
		updateTuneTracks();
	}
}
#endif
uint QompPlayListModel::totalTime() const
{
	uint total = 0;
	foreach(Tune* t, tunes_) {
		total += Qomp::durationStringToSeconds(t->duration);
	}
	return total;
}

void QompPlayListModel::updateTuneTracks()
{
	if(rowCount() == 0)
		return;

	for(int i = 0; i < rowCount(); ++i) {
		tunes_.at(i)->trackNumber = QString::number(i+1);
	}
	emit dataChanged(indexForTune(tunes_.at(0)), indexForTune(tunes_.at(rowCount() - 1)));
}
