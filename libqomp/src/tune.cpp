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

#include "tune.h"
#include "pluginmanager.h"
#include "covercache.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QFileInfo>

static const QString separator = "@qomp@";
static const QString simpleStrategyName = "SimpleStrategy";
static const QString streamVersion = "2.0";


TuneURLResolveStrategy::TuneURLResolveStrategy(QObject *p) :
	QObject(p)
{
}



class SimpleStrategy : public TuneURLResolveStrategy
{
public:
	static SimpleStrategy* instance()
	{
		if(!instance_)
			instance_ = new SimpleStrategy;
		return instance_;
	}

	virtual QUrl getUrl(const Tune *t)
	{
		if(!t->file.isEmpty())
			return QUrl::fromLocalFile(t->file);

		return QUrl(t->url);
	}

	virtual QString name() const
	{
		return simpleStrategyName;
	}

private:
	static SimpleStrategy* instance_;
	SimpleStrategy() : TuneURLResolveStrategy(QCoreApplication::instance()){}
};

SimpleStrategy* SimpleStrategy::instance_ = 0;




Tune::Tune(bool canSave) :
	QObject(),
	start(0),
	length(0),
	played(false),
	canSave_(canSave),
	strategy_(0),
	metadataResolved_(false)
{
	id_ = lastId_++;
	setUrlResolveStrategy(SimpleStrategy::instance());
}

Tune::~Tune()
{
}

QString Tune::displayString() const
{
	QString ret;
	if(!trackNumber.isEmpty()) {
		ret = trackNumber+".";
	}
	if(!title.isEmpty()) {
		if(!artist.isEmpty()) {
			ret += artist+" - ";
		}
		ret += title;
		if(!duration.isEmpty()) {
			ret += QString("    [%1]").arg(duration);
		}
	}
	else {
		if(file.isEmpty())
			ret += url;
		else {
			QFileInfo fi(file);
			ret += fi.baseName();
		}
		if(!duration.isEmpty()) {
			ret += QString("    [%1]").arg(duration);
		}
	}
	return ret;
}

QUrl Tune::getUrl() const
{
	return strategy()->getUrl(this);
}

QString Tune::toString() const
{
	QStringList list;
	list << artist << title << trackNumber << album << duration << url << file
	     << strategy()->name() << (canSave_ ? "true" : "false") << bitRate
	     << QString::number(start) << QString::number(length);

	if(!cover_.isNull())
		list.append( *cover_.data() );
	else
		list.append(QStringLiteral(""));

	list << directUrl << description << genre << streamVersion;

	return list.join(separator);
}

bool Tune::fromString(const QString &str)
{
	QStringList list = str.split(separator);
	if(list.size() < 8)
		return false;

	auto legacyFromString = [this, &list]() -> bool {
		artist = list.takeFirst();
		title = list.takeFirst();
		trackNumber = list.takeFirst();
		album = list.takeFirst();
		duration = list.takeFirst();
		url = list.takeFirst();
		file = list.takeFirst();

		const QString strName = list.takeFirst();
		if(strName != simpleStrategyName) {
			TuneURLResolveStrategy *rs = PluginManager::instance()->urlResolveStrategy(strName);
			if(rs)
				setUrlResolveStrategy(rs);
		}

		if(!list.isEmpty())
			canSave_ = (list.takeFirst() == "true");
		if(!list.isEmpty())
			bitRate = list.takeFirst();

		if(!list.isEmpty())
			start = list.takeFirst().toLongLong();
		if(!list.isEmpty())
			length = list.takeFirst().toLongLong();
		if(!list.isEmpty())
			cover_ = CoverCache::instance()->restore(list.takeFirst());

		if(!cover_.isNull())
			metadataResolved_ = true;

		return true;
	};

	if(list.last().compare(streamVersion) != 0)
		return legacyFromString();

	artist = list.takeFirst();
	title = list.takeFirst();
	trackNumber = list.takeFirst();
	album = list.takeFirst();
	duration = list.takeFirst();
	url = list.takeFirst();
	file = list.takeFirst();

	const QString strName = list.takeFirst();
	if(strName != simpleStrategyName) {
		TuneURLResolveStrategy *rs = PluginManager::instance()->urlResolveStrategy(strName);
		if(rs)
			setUrlResolveStrategy(rs);
	}

	canSave_ = (list.takeFirst() == "true");
	bitRate = list.takeFirst();
	start = list.takeFirst().toLongLong();
	length = list.takeFirst().toLongLong();

	cover_ = CoverCache::instance()->restore(list.takeFirst());
	if(!cover_.isNull())
		metadataResolved_ = true;

	directUrl = list.takeFirst();
	description = list.takeFirst();
	genre = list.takeFirst();

	return true;
}

int Tune::id() const
{
	return id_;
}

void Tune::setUrlResolveStrategy(TuneURLResolveStrategy *strategy) const
{
	if(strategy_ != strategy) {
		strategy_ = strategy;
	}
}

TuneURLResolveStrategy *Tune::strategy() const
{
	if(!strategy_)
		setUrlResolveStrategy(SimpleStrategy::instance());

	return strategy_;
}

bool Tune::operator==(const Tune& other) const
{
	return id() == other.id();
}

QList<Tune*> Tune::tunesFromFile(const QString &fileName)
{
	QList<Tune*> tunes;
	QFile f(fileName);
	if(f.exists() && f.open(QFile::ReadOnly)) {
		QTextStream str(&f);
		str.setCodec("UTF-8");
		while(!str.atEnd()) {
			Tune* t = new Tune;
			if(t->fromString(str.readLine())) {
				tunes.append(t);
			}
		}
	}
	return tunes;
}

bool Tune::sameSource(const Tune *other) const
{
	return QString::compare(other->file, file) == 0 && QString::compare(other->url, url) == 0;
}

#ifdef	QOMP_MOBILE
const QString Tune::cover() const
{
	if(!cover_.isNull()) {
		QString* hash = cover_.data();
		return QStringLiteral("image://tune/") + *hash;
	}

	return QString("image://tune/default"); //this may be any string
}
#else
const QImage Tune::cover() const
{
	if(!cover_.isNull()) {
		QString* hash = cover_.data();
		return CoverCache::instance()->get(*hash);
	}

	return CoverCache::instance()->defaultImage();
}
#endif
void Tune::setCover(const QImage &cover)
{
	cover_ = CoverCache::instance()->put(cover);
}

void Tune::setMetadataResolved(bool resolved)
{
	metadataResolved_ = resolved;
}

int Tune::lastId_ = 0;
const Tune* Tune::empty_ = new Tune(false);
