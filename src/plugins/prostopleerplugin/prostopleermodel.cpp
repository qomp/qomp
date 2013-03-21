#include "prostopleermodel.h"
#include <QTime>

ProstopleerModel::ProstopleerModel(QObject *parent) :
	QAbstractListModel(parent)
{
}

void ProstopleerModel::addTunes(const QList<ProstopleerTune> &tunes)
{
	emit beginInsertRows(QModelIndex(), tunes_.size(), tunes_.size()+tunes.size());
	tunes_.append(tunes);
	emit endInsertRows();
}

ProstopleerTune ProstopleerModel::tune(const QModelIndex &index) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return ProstopleerTune();

	return tunes_.at(index.row());
}

QVariant ProstopleerModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || index.row() >= tunes_.size())
		return QVariant();

	if(index.column() == 0) {
		if(role == Qt::DisplayRole) {
			ProstopleerTune t = tunes_.at(index.row());
			int sec = t.duration.toInt();
			int h = sec / (60*60);
			sec -= h*(60*60);
			int m = sec / 60;
			sec -= m*60;
			QTime time(h, m, sec, 0);
			const QString format = h ? "hh:mm:ss" : "mm:ss";
			QString ret = QString("%1 - %2  %3").arg(t.artist, t.title, time.toString(format));
			if(!t.url.isNull())
				ret += "  [OK]";
			return ret;
		}
		else if(role == Qt::CheckStateRole) {
			return QVariant(selected_.contains(index) ? 2 : 0);
		}
	}
	return QVariant();
}

int ProstopleerModel::rowCount(const QModelIndex &/*parent*/) const
{
	return tunes_.size();
}

Qt::ItemFlags ProstopleerModel::flags(const QModelIndex& index) const
{
	if(!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
}

void ProstopleerModel::urlChanged(const QString &id, const QString &url)
{
	QList<ProstopleerTune>::iterator it = tunes_.begin();
	for(; it != tunes_.end(); ++it) {
		if((*it).id == id) {
			emit layoutAboutToBeChanged();
			(*it).url = url;
			emit layoutChanged();
			break;
		}
	}
}

void ProstopleerModel::reset()
{
	beginResetModel();
	tunes_.clear();
	selected_.clear();
	endResetModel();
}

bool ProstopleerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid() || role != Qt::EditRole || index.column() != 0)
		return false;

	switch(value.toInt()) {
	case(0):
		if(selected_.contains(index))
			selected_.remove(index);
		break;
	case(2):
		if(!selected_.contains(index))
			selected_ << index;
		break;
	case(3):
		if(selected_.contains(index))
			selected_.remove(index);
		else
			selected_ << index;
		break;
	}

	emit dataChanged(index, index);

	return true;
}
