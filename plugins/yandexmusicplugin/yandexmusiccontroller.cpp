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
#include "qompplugincaptchadialog.h"

#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>
#include <QEventLoop>
#include <QPixmap>

#ifdef HAVE_QT5
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#else
#include <qjson/parser.h>
#endif

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

static const QString YA_MUSIC_URL("https://music.yandex.ru/");
static const QString ARTISTS_NAME("artists");
static const QString ALBUMS_NAME("albums");
static const QString TRACKS_NAME("tracks");

#ifndef HAVE_QT5
typedef QVariant QJsonValue;
typedef QVariantList QJsonArray;
typedef QVariantMap QJsonObject;

#define toObject() toMap()
#define toArray() toList()
#endif

static QString safeJSONValue2String(const QJsonValue& val)
{
#ifdef HAVE_QT5
	switch (val.type()) {
	case QJsonValue::String:
		return val.toString();
	case QJsonValue::Double:
		return QString::number(val.toInt());
	default:
		break;
	}
#else
	return val.toString();
#endif
	return QString();
}

static QJsonArray ByteArrayToJsonArray(const QString& type, const QByteArray& ba)
{
#ifdef HAVE_QT5
	QJsonDocument doc = QJsonDocument::fromJson(ba);
	QJsonObject jo = doc.object();
	QJsonObject art = jo.value(type).toObject();
	QJsonArray arr = art.value("items").toArray();
	return arr;
#else
	QJson::Parser parser;
	bool ok;
	QJsonArray arr;
	QJsonObject result = parser.parse(ba, &ok).toMap();
	if (ok) {
		QJsonObject t = result[type].toMap();
		arr = t["items"].toList();
	}
	return arr;
#endif
}

//----------------------------------------
//---------YandexMusicTune----------------
//----------------------------------------
class YandexMusicTune : public QompPluginTune
{
public:
	explicit YandexMusicTune(QompPluginModelItem* parent = 0) :
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
	dlg_(new YandexMusicGettunsDlg()),
	mainUrl_(YA_MUSIC_URL)
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
	nam()->cookieJar()->setCookiesFromUrl(l, mainUrl_);

	connect(this, SIGNAL(queryFinished()), SLOT(makeQuery()));
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

	queries_.insert(ARTISTS_NAME, SLOT(artistsSearchFinished()));
	queries_.insert(ALBUMS_NAME, SLOT(albumsSearchFinished()));
	queries_.insert(TRACKS_NAME, SLOT(tracksSearchFinished()));

	searchText_ = txt;

	makeQuery();
}

QompPluginGettunesDlg *YandexMusicController::view() const
{
	return dlg_;
}

void YandexMusicController::search(const QString& text, const QString &type, const char *slot, int page)
{
	static const QString urlStr = mainUrl_+ QString("handlers/music-search.jsx?text=%1&type=%2");
	QString url = urlStr.arg(text, type);
	if(page > 0) {
		url += "&page=" + QString::number(page);
	}
	QNetworkReply* reply = nam()->get(creatNetworkRequest(QUrl(url)));
	connect(reply, SIGNAL(finished()), slot);
	requests_.insert(reply, nullptr);
	dlg_->startBusyWidget();
}

void YandexMusicController::searchNextPage(const QByteArray &reply, const QString &type, const char *slot)
{
#ifdef HAVE_QT5
	QJsonDocument doc = QJsonDocument::fromJson(reply);
	QJsonObject root = doc.object();
	if(!root.contains("pager"))
		return;
#else
	QJson::Parser parser;
	bool ok;
	QJsonObject root = parser.parse(reply, &ok).toMap();
	if (!ok || !root.contains("pager")) {
		return;
	}
#endif
	QJsonObject pages = root.value("pager").toObject();
	int curPage = pages.value("page").toInt();
	int total = pages.value("total").toInt();
	int perPage = pages.value("perPage").toInt();
	if (total / perPage >= curPage)
		search(root.value("text").toString(), type, slot, ++curPage);
}

QNetworkRequest YandexMusicController::creatNetworkRequest(const QUrl &url) const
{
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	nr.setRawHeader("Referer", mainUrl_.toLatin1());
	return nr;
}

bool YandexMusicController::checkRedirect(QNetworkReply *reply, const char* slot, QompPluginTreeModel *model)
{
	if(reply->header(QNetworkRequest::LocationHeader).isValid()) {
		QString str = reply->header(QNetworkRequest::LocationHeader).toString();

#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicController::checkRedirect() \n  url:\n" << reply->url().toString()
							<< "\n  location:\n" << str;
#endif

		QUrl url(str);
		mainUrl_ = QString("%1://%2/").arg(url.scheme(), url.host());

		QNetworkRequest nr = creatNetworkRequest(url);
		QNetworkReply* r = nam()->get(nr);
		connect(r, SIGNAL(finished()), slot);
		requests_.insert(r, model);
		dlg_->startBusyWidget();

		return true;
	}

	return false;
}

bool YandexMusicController::checkCaptcha(const QUrl& replyUrl, const QByteArray &reply,
					 const char *slot, QompPluginTreeModel *model)
{
#ifdef HAVE_QT5
	QJsonDocument doc = QJsonDocument::fromJson(reply);
	QJsonObject root = doc.object();
	if(!root.contains("type") || root.value("type").toString() != QStringLiteral("captcha"))
		return false;
#else
	QJson::Parser parser;
	bool ok;
	QJsonObject root = parser.parse(reply, &ok).toMap();
	if (!ok || !root.contains("type") || root.value("type").toString() != QString("captcha")) {
		return false;
	}
#endif

	QJsonObject captcha = root.value("captcha").toObject();

	const QString status = captcha.value("status").toString();
	if(status == "success") {
		return true;
	}

	const QString imageURL = captcha.value("img-url").toString();
	const QString page = captcha.value("captcha-page").toString();
	const QString ref = QUrl(page).query(QUrl::FullyEncoded);
	QString key = captcha.value("key").toString();

#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicController::checkCaptcha():  " << imageURL <<  key << page << ref;
#endif

	QString key2;
	QPixmap px = getCaptcha(imageURL, &key2);
	QompPluginCaptchaDialog dlg(px, dlg_);
	if(dlg.start()) {
		QUrl url(QString("%1://%2/checkcaptcha?key=%3&%4&rep=%5")
				.arg(replyUrl.scheme(), replyUrl.host(), key, ref,
				QUrl::toPercentEncoding(dlg.result())), QUrl::StrictMode);
#ifdef DEBUG_OUTPUT
		qDebug() << url.toString(QUrl::FullyEncoded);
#endif
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
		QNetworkReply* r = nam()->get(nr);
		connect(r, SIGNAL(finished()), slot);
		requests_.insert(r, model);
		dlg_->startBusyWidget();
	}

	return true;
}

QPixmap YandexMusicController::getCaptcha(const QString &captchaUrl, QString *key)
{
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicController::getCaptcha():  " << captchaUrl;
#endif
	QUrl url(captchaUrl);
	QNetworkRequest nr = creatNetworkRequest(url);
	QNetworkReply* r = nam()->get(nr);

	dlg_->startBusyWidget();
	QEventLoop el;
	connect(r, SIGNAL(finished()), &el, SLOT(quit()));
	el.exec();
	dlg_->stopBusyWidget();
	r->deleteLater();

	QPixmap pix;
	if (r->error() == QNetworkReply::NoError) {
		if(r->header(QNetworkRequest::LocationHeader).isValid()) {
			QString str = r->header(QNetworkRequest::LocationHeader).toString();
			return getCaptcha(str, key);
		}
		if(url.hasQuery()) {
			const QString queries = url.query();
			foreach(const QString& query, queries.split("&")) {
				QStringList data = query.split("=");
				if(data.at(0) == "key" && data.size() == 2) {
					*key = data.at(1);
				}
			}
		}
		QByteArray ba = r->readAll();
		pix.loadFromData(ba);
	}

	return pix;
}

void YandexMusicController::artistsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		if(checkRedirect(reply, SLOT(artistsSearchFinished()))) {
			return;
		}

		QByteArray ba = reply->readAll();
#ifdef DEBUG_OUTPUT
		qDebug() << ba;
#endif

		if(checkCaptcha(reply->url(), ba, SLOT(artistsSearchFinished()))) {
			return;
		}

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
#ifdef DEBUG_OUTPUT
	else {
		qDebug() << reply->errorString();
	}
#endif

	emit queryFinished();
}

void YandexMusicController::albumsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		if(checkRedirect(reply, SLOT(albumsSearchFinished()))) {
			return;
		}

		const QByteArray ba = reply->readAll();

		if(checkCaptcha(reply->url(), ba, SLOT(albumsSearchFinished()))) {
			return;
		}

		QJsonArray arr = ByteArrayToJsonArray(ALBUMS_NAME, ba);
		albumsModel_->addTopLevelItems(parseAlbums(arr));

		searchNextPage(ba, ALBUMS_NAME, SLOT(albumsSearchFinished()));
	}
	else {
#ifdef DEBUG_OUTPUT
	qDebug() << "albumssSearchFinished()" << reply->errorString();
#endif
	}

	emit queryFinished();
}

void YandexMusicController::tracksSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		if(checkRedirect(reply, SLOT(tracksSearchFinished()))) {
			return;
		}

		const QByteArray ba = reply->readAll();
		if(checkCaptcha(reply->url(), ba, SLOT(tracksSearchFinished()))) {
			return;
		}

		QJsonArray arr = ByteArrayToJsonArray(TRACKS_NAME, ba);
		tracksModel_->addTopLevelItems(parseTunes(arr));

		searchNextPage(ba, TRACKS_NAME, SLOT(tracksSearchFinished()));
	}
	else {
#ifdef DEBUG_OUTPUT
	qDebug() << "tracksSearchFinished()" << reply->errorString();
#endif
	}

	emit queryFinished();
}

void YandexMusicController::artistUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		if(checkRedirect(reply, SLOT(artistUrlFinished()))) {
			return;
		}

		const QByteArray replyStr = reply->readAll();
		if(checkCaptcha(reply->url(), replyStr, SLOT(artistUrlFinished()))) {
			return;
		}

#ifdef DEBUG_OUTPUT
		qDebug() << replyStr;
#endif
#ifdef HAVE_QT5
		QJsonDocument doc = QJsonDocument::fromJson(replyStr);
		QJsonObject jo = doc.object();
	#ifdef DEBUG_OUTPUT
		qDebug() << doc.toJson(QJsonDocument::Indented);
	#endif
#else
		QJson::Parser parser;
		bool ok;
		QJsonObject jo = parser.parse(replyStr, &ok).toMap();
		if (!ok) {
			return;
		}
#endif
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
	else {
#ifdef DEBUG_OUTPUT
	qDebug() << "artistUrlFinished()" << reply->errorString();
#endif
	}
}

void YandexMusicController::makeQuery()
{
	if(queries_.isEmpty())
		return;

	const QString key = queries_.keys().first();
	search(searchText_, key, queries_.take(key));
}

void YandexMusicController::albumUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	void* model = requests_.value(reply);
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		if(checkRedirect(reply, SLOT(albumUrlFinished()), static_cast<QompPluginTreeModel*>(model))) {
			return;
		}

		const QByteArray replyStr = reply->readAll();
		if(checkCaptcha(reply->url(), replyStr, SLOT(albumUrlFinished()), static_cast<QompPluginTreeModel*>(model))) {
			return;
		}

#ifdef HAVE_QT5
		QJsonDocument doc = QJsonDocument::fromJson(replyStr);
		QJsonObject jo = doc.object();
	#ifdef DEBUG_OUTPUT
		qDebug() << doc.toJson(QJsonDocument::Indented);
	#endif
#else
		QJson::Parser parser;
		bool ok;
		QJsonObject jo = parser.parse(replyStr, &ok).toMap();
		if (!ok) {
			return;
		}
#endif
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
			QompPluginTreeModel *model_ = static_cast<QompPluginTreeModel *>(model);
			QompPluginModelItem* it = model_->itemForId(reply->property("id").toString());
			model_->setItems(list, it);

			QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
			pa->tunesReceived = true;
		}
	}
	else {
#ifdef DEBUG_OUTPUT
	qDebug() << "albumUrlFinished()" << reply->errorString();
#endif
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

	QUrl url(mainUrl_ + path, QUrl::StrictMode);
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
