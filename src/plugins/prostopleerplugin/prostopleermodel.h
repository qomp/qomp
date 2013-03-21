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

	bool operator==(const ProstopleerTune& other) {
		return id == other.id;
	}
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
