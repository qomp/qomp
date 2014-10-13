/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "yandexmusiccontroller.h"
#include "yandexmusicgettunsdlg.h"
#include "qompplugintreemodel.h"
#include "qompplugintypes.h"
#include "common.h"
#include "yandexmusicurlresolvestrategy.h"

#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

static const QString YA_MUSIC_URL("http://music.yandex.ru/");
static const QString ARTISTS_NAME("artists");
static const QString ALBUMS_NAME("albums");
static const QString TRACKS_NAME("tracks");

static QString safeJSONValue2String(const QJsonValue& val)
{
	switch (val.type()) {
	case QJsonValue::String:
		return val.toString();
	case QJsonValue::Double:
		return QString::number(val.toInt());
	default:
		break;
	}
	return QString();
}

static QNetworkRequest creatNetworkRequest(const QUrl& url)
{
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	nr.setRawHeader("Referer", YA_MUSIC_URL.toLatin1());
	return nr;
}

static QJsonArray ByteArrayToJsonArray(const QString& type, const QByteArray& ba)
{
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	QJsonObject jo = doc.object();
	QJsonObject art = jo.value(type).toObject();
	QJsonArray arr = art.value("items").toArray();
	return arr;
}

//----------------------------------------
//---------YandexMusicTune----------------
//----------------------------------------
class YandexMusicTune : public QompPluginTune
{
public:
	YandexMusicTune(QompPluginModelItem* parent = 0) :
		QompPluginTune(parent) {}

	QString durationToString() const
	{
		return Qomp::durationMiliSecondsToString(duration.toLongLong());
	}

	virtual Tune* toTune() const
	{
		Tune* t = QompPluginTune::toTune();
		t->duration = durationToString();
		return t;
	}
};


static QList<QompPluginModelItem*> parseAlbums(const QJsonArray& arr)
{
	QList<QompPluginModelItem*> albums;
	for(int i = 0; i < arr.count(); ++i) {
		QompPluginAlbum* album = new QompPluginAlbum;
		QJsonObject cur = arr.at(i).toObject();
		album->album = cur.value("title").toString();
		album->internalId = safeJSONValue2String(cur.value("id"));
		if(cur.contains("year"))
			album->year = safeJSONValue2String(cur.value("year"));
		if(cur.contains(ARTISTS_NAME)){
			QJsonArray ja = cur.value(ARTISTS_NAME).toArray();
			if(!ja.isEmpty())
				album->artist = ja.first().toObject().value("name").toString();
		}
		int count = 1;
		if(cur.contains("trackCount"))
			count = cur.value("trackCount").toInt();
		QList<QompPluginModelItem*> fakeItems;
		do {
			fakeItems << new QompPluginTune(); //add fake item
		} while(--count > 1);

		album->setItems(fakeItems);
		albums.append(album);
	}
	return albums;
}

static QList<QompPluginModelItem*> parseTunes(const QJsonArray& arr)
{
	QList<QompPluginModelItem*> tracks;
	for(int i = 0; i < arr.count(); ++i) {
		YandexMusicTune* tune = new YandexMusicTune;
		QJsonObject cur = arr.at(i).toObject();
		tune->title = cur.value("title").toString();
		if(cur.contains(ARTISTS_NAME)){
			QJsonArray ja = cur.value(ARTISTS_NAME).toArray();
			if(!ja.isEmpty())
				tune->artist = ja.first().toObject().value("name").toString();
		}
		if(cur.contains(ALBUMS_NAME)){
			QJsonArray ja = cur.value(ALBUMS_NAME).toArray();
			if(!ja.isEmpty())
				tune->artist = ja.first().toObject().value("title").toString();
		}
		tune->duration = safeJSONValue2String(cur.value("durationMs"));
		tune->internalId = safeJSONValue2String(cur.value("id"));
		tune->url = cur.value("storageDir").toString();
		tracks.append(tune);
	}
	return tracks;
}




//----------------------------------------
//--------YandexMusicController-----------
//----------------------------------------
YandexMusicController::YandexMusicController(QObject *parent) :
	QompPluginController(parent),
	tracksModel_(new QompPluginTreeModel(this)),
	albumsModel_(new QompPluginTreeModel(this)),
	artistsModel_(new QompPluginTreeModel(this)),
	dlg_(new YandexMusicGettunsDlg())
{
	init();
}

YandexMusicController::~YandexMusicController()
{
	delete dlg_;
}

void YandexMusicController::init()
{
	QompPluginController::init();
	dlg_->setModel(tracksModel_, TabTrack);
	dlg_->setModel(albumsModel_, TabAlbum);
	dlg_->setModel(artistsModel_, TabArtist);
	dlg_->setCuuretnTab(TabArtist);
	QNetworkCookie c("makeyourownkindofmusic", "optin");
	QList<QNetworkCookie> l;
	l.append(c);
	nam()->cookieJar()->setCookiesFromUrl(l, YA_MUSIC_URL);
}

QList<Tune*> YandexMusicController::prepareTunes() const
{
	QList<Tune*> tunes;
	QList<QompPluginModelItem*> list = QList<QompPluginModelItem*>()
			<< artistsModel_->selectedItems()
			<< albumsModel_->selectedItems()
			<< tracksModel_->selectedItems();

	foreach(QompPluginModelItem* tune, list) {
		if(!tune || tune->type() != QompCon::TypeTune)
			continue;

		QompPluginTune* pt = static_cast<QompPluginTune*>(tune);
		if(pt->url.isEmpty())
			continue;

		Tune* tune_ = pt->toTune();
		tune_->setUrlResolveStrategy(YandexMusicURLResolveStrategy::instance());
		tunes.append(tune_);
	}
	return tunes;
}

void YandexMusicController::doSearch(const QString& txt)
{
	if(txt.isEmpty())
		return;

	foreach(QNetworkReply* r, requests_.keys()) {
		r->deleteLater();
	}
	requests_.clear();

	albumsModel_->reset();
	artistsModel_->reset();
	tracksModel_->reset();

	QHash<QString, const char*> req;
	req.insert(ARTISTS_NAME, SLOT(artistsSearchFinished()));
	req.insert(ALBUMS_NAME, SLOT(albumsSearchFinished()));
	req.insert(TRACKS_NAME, SLOT(tracksSearchFinished()));
	foreach(const QString& key, req.keys()) {
		search(txt, key, req.value(key));
	}
}

QompPluginGettunesDlg *YandexMusicController::view() const
{
	return dlg_;
}

void YandexMusicController::search(const QString& text, const QString &type, const char *slot, int page)
{
	static const QString urlStr = YA_MUSIC_URL + QString("handlers/search.jsx?text=%1&type=%2");
	QString url = urlStr.arg(text, type);
	if(page > 0) {
		url += "&page=" + QString::number(page);
	}
	QNetworkReply* reply = nam()->get(creatNetworkRequest(QUrl(url)));
	connect(reply, SIGNAL(finished()), slot);
	requests_.insert(reply, 0);
	dlg_->startBusyWidget();
}

void YandexMusicController::searchNextPage(const QByteArray &reply, const QString &type, const char *slot)
{
	QJsonDocument doc = QJsonDocument::fromJson(reply);
	QJsonObject root = doc.object();
	if(!root.contains("pager"))
		return;

	QJsonObject pages = root.value("pager").toObject();
	int curPage = pages.value("page").toInt();
	int total = pages.value("total").toInt();
	int perPage = pages.value("perPage").toInt();
	if (total / perPage >= curPage)
		search(root.value("text").toString(), type, slot, ++curPage);
}

void YandexMusicController::artistsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QByteArray ba = reply->readAll();
#ifdef DEBUG_OUTPUT
		qDebug() << ba;
#endif

		QJsonArray arr = ByteArrayToJsonArray(ARTISTS_NAME, ba);
		QList<QompPluginModelItem*> artists;

		for(int i = 0; i < arr.count(); ++i) {
			QompPluginArtist* artist = new QompPluginArtist;
			QJsonObject cur = arr.at(i).toObject();
			artist->artist = Qomp::unescape(cur.value("name").toString());
			artist->internalId = safeJSONValue2String(cur.value("id"));
			artist->setItems(QList<QompPluginModelItem*>() << new QompPluginTune()); //add fake item
			artists.append(artist);
		}
		if(!artists.isEmpty()) {
			artistsModel_->addTopLevelItems(artists);
		}

		searchNextPage(ba, ARTISTS_NAME, SLOT(artistsSearchFinished()));
	}
}

void YandexMusicController::albumsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		const QByteArray ba = reply->readAll();
		QJsonArray arr = ByteArrayToJsonArray(ALBUMS_NAME, ba);
		albumsModel_->addTopLevelItems(parseAlbums(arr));

		searchNextPage(ba, ALBUMS_NAME, SLOT(albumsSearchFinished()));
	}
}

void YandexMusicController::tracksSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		const QByteArray ba = reply->readAll();
		QJsonArray arr = ByteArrayToJsonArray(TRACKS_NAME, ba);
		tracksModel_->addTopLevelItems(parseTunes(arr));

		searchNextPage(ba, TRACKS_NAME, SLOT(tracksSearchFinished()));
	}
}

void YandexMusicController::artistUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();
	if(reply->error() == QNetworkReply::NoError) {
		const QByteArray replyStr = reply->readAll();
#ifdef DEBUG_OUTPUT
		qDebug() << replyStr;
#endif
		QJsonDocument doc = QJsonDocument::fromJson(replyStr);
#ifdef DEBUG_OUTPUT
		qDebug() << doc.toJson(QJsonDocument::Indented);
#endif
		QJsonObject jo = doc.object();
		QJsonArray arr = jo.value(ALBUMS_NAME).toArray();
		QJsonArray also = jo.value("alsoAlbums").toArray();

		QList<QompPluginModelItem*> list;
		list = parseAlbums(arr);
		list += parseAlbums(also);
		if(!list.isEmpty()) {
			QompPluginModelItem* it = artistsModel_->itemForId(reply->property("id").toString());
			artistsModel_->setItems(list, it);
			QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
			pa->tunesReceived = true;
		}
	}
}

void YandexMusicController::albumUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	void* model = requests_.value(reply);
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		const QByteArray replyStr = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(replyStr);
#ifdef DEBUG_OUTPUT
		qDebug() << doc.toJson(QJsonDocument::Indented);
#endif
		QJsonObject jo = doc.object();
		QJsonArray arr = jo.value("volumes").toArray();
		QList<QompPluginModelItem*> list;
		while(!arr.isEmpty())
			list += parseTunes(arr.takeAt(0).toArray());

		if(!list.isEmpty()) {
			QJsonArray art = jo.value(ARTISTS_NAME).toArray();
			if(!art.isEmpty()) {
				QString artist = art.first().toObject().value("name").toString();
				foreach (QompPluginModelItem* it, list) {
					static_cast<QompPluginTune*>(it)->artist = artist;
				}
			}
			QompPluginTreeModel *model_ = (QompPluginTreeModel *)model;
			QompPluginModelItem* it = model_->itemForId(reply->property("id").toString());
			model_->setItems(list, it);

			QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
			pa->tunesReceived = true;
		}
	}
}

void YandexMusicController::itemSelected(QompPluginModelItem *item)
{
	if(!item || item->internalId.isEmpty())
		return;

	QompPluginTreeModel* model = qobject_cast<QompPluginTreeModel*>(item->model());

	QString path;
	const char* slot = 0;
	switch(item->type()) {
	case QompCon::TypeAlbum:
	{
		QompPluginAlbum *album = static_cast<QompPluginAlbum *>(item);
		if(album->tunesReceived)
			return;
		path = QUrl::fromPercentEncoding(QString("handlers/album.jsx?album=%1")
						 .arg(album->internalId).toLatin1());
		slot = SLOT(albumUrlFinished());
		break;
	}
	case QompCon::TypeArtist:
	{
		QompPluginArtist *artist = static_cast<QompPluginArtist *>(item);
		if(artist->tunesReceived)
			return;
		path = QUrl::fromPercentEncoding(QString("handlers/artist.jsx?artist=%1&what=albums&sort=year")
						 .arg(item->internalId).toLatin1());
		slot = SLOT(artistUrlFinished());
		break;
	}
	default:
		return;
	}

	QUrl url(YA_MUSIC_URL + path, QUrl::StrictMode);
	QNetworkReply *reply = nam()->get(creatNetworkRequest(url));
	reply->setProperty("id", item->internalId);

	requests_.insert(reply, model);
	connect(reply, SIGNAL(finished()), slot);
	dlg_->startBusyWidget();
}

void YandexMusicController::getSuggestions(const QString &text)
{
	static const QString urlStr = "http://suggest-music.yandex.ru/suggest-ya.cgi?v=4&part=%1";
	QNetworkRequest nr = creatNetworkRequest(urlStr.arg(text));
	QNetworkReply* reply = nam()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(suggestionsFinished()));
}

void YandexMusicController::suggestionsFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
#ifdef DEBUG_OUTPUT
		qDebug() << replyStr;
#endif
		QRegExp sugRx("\\[([^\\]]+)\\]");
		if(sugRx.indexIn(replyStr) != -1 ) {
			QStringList sugs = sugRx.cap(1).split(",");
			if(!sugs.isEmpty()) {
				for(QStringList::Iterator it = sugs.begin(); it != sugs.end(); ++ it) {
					*it = (*it).mid(1, (*it).size() - 2);
				}
				emit suggestionsReady(sugs);
			}
		}
	}
}

void YandexMusicController::checkAndStopBusyWidget()
{
	if(!requests_.isEmpty())
		return;

	dlg_->stopBusyWidget();

	//this is not the best place for this, but....
	if(dlg_->currentTabRows() != 0)
		return;

	if(artistsModel_->rowCount()) {
		dlg_->setCuuretnTab(TabArtist);
	}
	else if(albumsModel_->rowCount()) {
		dlg_->setCuuretnTab(TabAlbum);
	}
	else if(tracksModel_->rowCount()) {
		dlg_->setCuuretnTab(TabTrack);
	}
}
