/*
 * Copyright (C) 2014-2016  Khryukin Evgeny
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

#include "myzukarucontroller.h"
#include "myzukarugettunesdlg.h"
#include "qompplugintypes.h"
#include "common.h"
#include "myzukarudefines.h"
#include "qompplugintreemodel.h"
#include "myzukaruresolvestrategy.h"
#include "tune.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>
#ifdef HAVE_QT6
#include <QRegularExpression>
#endif
#include <QStringList>
#include <QtConcurrent/QtConcurrentRun>
#include <QFutureWatcher>

#include <algorithm>

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif


static const QString artistsRegExp = QString(
		"<tr>\\s+"
		"<td .+"
		"</td>\\s+"
		"<td>\\s+"
		"<a\\s+(Class=\"darkorange\"\\s+)?href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+"		//cap(2) - internalID, cap(3) - Artist
		"</td>\\s+"
		"<td>\\s+"
		"(\\d+)\\s+"									//cap(4) - albums count
		"</td>\\s+</tr>");

static const QString albumsRegExp = QString(
		"<tr>\\s+"
		"<td .+"
		"</td>\\s+"
		"<td>\\s+"
		"<a\\s+(Class=\"darkorange\"\\s+)?href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+"		//cap(3) - Artist
		"</td>\\s+"
		"<td>\\s+"
		"<a\\s+(class=\"darkorange\"\\s+)?href=\"(/Album/[^\"]+)\">"				//cap(5) - internalId
		"([^<]+)"									//cap(6) - Album
		"</a>\\s+"
		"</td>\\s+"
		"<td>\\s+"
		"(\\d+)\\s+"									//cap(7) - year
		"</td>\\s+"
		"<td>\\s+"
		"(\\d+)\\s+"									//cap(8) - songs count
		"</td>\\s+"
		".+ </tr>"
		);

static const QString albumsRegExp2 = QString::fromUtf8(
			"<div\\s+data-type=\"([\\d]+)\"[^>]+>\\s+"				//cap(1) - album type
			"<div[^>]*>\\s+"							//(2,3,4,6 - direct albums, other - compilations)
			"<a[^>]+>.+</a>\\s+"
			"<div[^>]*>\\s+"
			"<ul>.+"
			"</ul>\\s+"
			"</div>\\s+"
			"</div>\\s+"
			"<div[^>]*>.+"
			"<div class=\"title\">\\s*<a\\s+href=\"([^\"]+)\">([^<]+)</a>.+"	//cap(2) - internalId; cap(3) - title
			"<div class=\"tags\">Год релиза:\\s*<a[^>]+>([^<]+)</a>"		//cap(4) - year
			);

static const QString songsRegExp = QString(
		"<tr>\\s+"
		"<td .+"
		"<a\\s+(Class=\"darkorange\"\\s+)?href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+"		//cap(3) - artist
		"</td>\\s+"
		"<td>\\s+"
		"<a\\s+(Class=\"darkorange\"\\s+)?href=\"(/Song/[^\"]+)\">([^<]+)</a>\\s+"	//cap(5) - internalID, cap(6) - Title
		"</td>\\s+"
		"<td>\\s+"
		"([^<]+)\\s+"									//cap(7) - size
		"</td>\\s+</tr>");

static const QString songsRegExp2 = QString(
	"<div[^>]+>\\s+"
	"<div[^>]+>\\s+"
	"<span\\s+class=\"[^\"]+\"\\s+data-url=\"/Song/Play/([^\"]+)\".+"					//cap(1) - internalId
	"</div>\\s+"
	"<div[^>]+>([^<]+)</div>.+"							//cap(2) - position
	"<div\\s+class=\"data\">([^<]+)<.+</div>\\s+"					//cap(3) - duration
	"</div>\\s+"
	"<div[^>]+>.+"
	"<a[^>]+>([^<]+)</a>.+"								//cap(4) - artist
	"<a[^>]+>([^<]+)</a>.+"								//cap(5) - title
		);


static bool lessThen(QompPluginModelItem* it1, QompPluginModelItem* it2)
{
	if(it1->type() == QompCon::TypeAlbum) {
		QompPluginAlbum* a1 = static_cast<QompPluginAlbum*>(it1);
		QompPluginAlbum* a2 = static_cast<QompPluginAlbum*>(it2);

//		if(a1->parent() && a2->parent()) {
//			QompPluginArtist* ar1 = static_cast<QompPluginArtist*>(a1->parent());
//			QompPluginArtist* ar2 = static_cast<QompPluginArtist*>(a2->parent());

//			if(a1->artist != ar1->artist && a2->artist == ar2->artist)
//				return false;

//			if(a1->artist == ar1->artist && a2->artist != ar2->artist)
//				return true;
//		}

		if(a1->artist.isEmpty() && !a2->artist.isEmpty())
			return false;

		if(!a1->artist.isEmpty() && a2->artist.isEmpty())
			return true;

		if(a1->year != a2->year)
			return a1->year.toInt() > a2->year.toInt();

		return a1->album < a2->album;
	}

	return it1->toString() < it2->toString();
}

static void sortAlbums(QList<QompPluginModelItem *> *items)
{
	if(items->size() > 1)
		std::sort(items->begin(), items->end(), lessThen);
}

static QList<QompPluginModelItem*> parseTunes(const QString& replyStr, int songIndex)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QList<QompPluginModelItem*> parseTunes" << replyStr;
#endif
	QList<QompPluginModelItem*> tunes;
	if(songIndex != -1) {
		QRegExp songRx(songsRegExp, Qt::CaseInsensitive);
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QompPluginTune* t = new QompPluginTune();
			t->internalId = songRx.cap(5);
			t->artist = Qomp::unescape(songRx.cap(3));
			t->title = Qomp::unescape(songRx.cap(6));
			QUrl u(MYZUKA_URL);
			u.setPath(QUrl::fromPercentEncoding(t->internalId.toLatin1()));
			t->directUrl = u.toString();
			//t->album = unescape(songRx.cap(9));

			tunes.append(t);
		}
	}
	return tunes;
}

static QList<QompPluginModelItem*> parseTunes2(const QString& replyStr, int songIndex)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QList<QompPluginModelItem*> parseTunes2" << replyStr;
#endif
	QList<QompPluginModelItem*> tunes;
	if(songIndex != -1) {
		QRegExp songRx(songsRegExp2, Qt::CaseInsensitive);
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QompPluginTune* t = new QompPluginTune();
			t->/*internalId*/url = Qomp::unescape(songRx.cap(1));
			t->artist = Qomp::unescape(songRx.cap(4));
			t->title = Qomp::unescape(songRx.cap(5));
			t->duration = Qomp::unescape(songRx.cap(3).trimmed());
			t->directUrl = QStringLiteral(MYZUKA_URL "Song/") + t->url.split("?").first();

			tunes.append(t);
		}
	}
	return tunes;
}

static QList<QompPluginModelItem*> parseAlbums(const QString& replyStr, int albumsIndex)
{
	QList<QompPluginModelItem*> albums;
	if(albumsIndex != -1) {
		QRegExp albumRx(albumsRegExp, Qt::CaseInsensitive);
		albumRx.setMinimal(true);
		while((albumsIndex = albumRx.indexIn(replyStr, albumsIndex)) != -1) {
			albumsIndex += albumRx.matchedLength();
			QompPluginAlbum* album = new QompPluginAlbum();
			album->artist = Qomp::unescape(albumRx.cap(3).trimmed());
			album->album = Qomp::unescape(albumRx.cap(6).trimmed());
			album->year = albumRx.cap(7);
			album->internalId = albumRx.cap(5).trimmed();

			//add fake empty items
			bool ok;
			int count = albumRx.cap(8).toInt(&ok);
			QList<QompPluginModelItem*> list;
			if(ok) {
				while(count--)
					list.append(new QompPluginTune(album));
			}
			album->setItems(list);
			albums.append(album);
		}
	}
	return albums;
}

static QList<QompPluginModelItem*> parseAlbums2(const QString& replyStr, int albumsIndex)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QList<QompPluginModelItem*> parseAlbums2" << replyStr;
#endif
	QList<QompPluginModelItem*> albums;
	if(albumsIndex != -1) {
		QRegExp albumRx(albumsRegExp2, Qt::CaseInsensitive);
		albumRx.setMinimal(true);
		while((albumsIndex = albumRx.indexIn(replyStr, albumsIndex)) != -1) {
			albumsIndex += albumRx.matchedLength();
			QompPluginAlbum* album = new QompPluginAlbum();
			album->artist = albumRx.cap(1); // actually contains album type (digital)
			album->album = Qomp::unescape(albumRx.cap(3).trimmed());
			album->year = albumRx.cap(4).trimmed();
			album->internalId = albumRx.cap(2).trimmed();

			//add fake empty item
			QList<QompPluginModelItem*> list;
			list.append(new QompPluginTune(album));
			album->setItems(list);
			albums.append(album);
		}
	}
	return albums;
}




MyzukaruController::MyzukaruController(QObject *parent) :
	QompPluginController(parent),
	tracksModel_(new QompPluginTreeModel(this)),
	albumsModel_(new QompPluginTreeModel(this)),
	artistsModel_(new QompPluginTreeModel(this)),
	dlg_(new MyzukaruGettunesDlg())
{
	init();
}

MyzukaruController::~MyzukaruController()
{
	delete dlg_;
}

void MyzukaruController::init()
{
	QompPluginController::init();

	dlg_->setModel(tracksModel_, TabTracks);
	dlg_->setModel(artistsModel_, TabArtists);
	dlg_->setModel(albumsModel_, TabAlbums);
	dlg_->setCurrentTab(TabArtists);
}

QList<Tune*> MyzukaruController::prepareTunes() const
{
	QList<Tune*> list;
	QList<QompPluginModelItem*> l = QList<QompPluginModelItem*>()
			<< artistsModel_->selectedItems()
			<< albumsModel_->selectedItems()
			<< tracksModel_->selectedItems();

	foreach(QompPluginModelItem* tune, l) {
		if(!tune || tune->type() != QompCon::TypeTune)
			continue;

		QompPluginTune* pt = static_cast<QompPluginTune*>(tune);
		if(pt->url.isEmpty())
			continue;

		Tune* t = pt->toTune();
		t->setUrlResolveStrategy(MyzukaruResolveStrategy::instance());

		list.append(t);
	}
	return list;
}

void MyzukaruController::doSearch(const QString &txt)
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

	const QString url = QString("%1Search?searchText=%2").arg(MYZUKA_URL,txt);
	QNetworkRequest nr;
	nr.setUrl(url);
	nr.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
	nr.setRawHeader("Accept-Language", "ru-RU,ru;q=0.8,en-US;q=0.6,en;q=0.4");
	nr.setRawHeader("Referer", MYZUKA_URL);
#ifndef HAVE_QT6
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif
	QNetworkReply* reply = nam()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
	connect(this, &MyzukaruController::destroyed, reply, &QNetworkReply::deleteLater);
	requests_.insert(reply, 0);
	dlg_->startBusyWidget();
}

QompPluginGettunesDlg *MyzukaruController::view() const
{
	return dlg_;
}

void MyzukaruController::searchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());

		int songIndex = replyStr.indexOf(QString::fromUtf8("<h1>Поиск по композициям</h1>"));
		QList<QompPluginModelItem*> tunes = parseTunes(replyStr, songIndex);
		if(!tunes.isEmpty()) {
			tracksModel_->addTopLevelItems(tunes);
		}

		int albumsIndex = replyStr.indexOf(QString::fromUtf8("<h1>Поиск по альбомам"));
		QList<QompPluginModelItem*> albums = parseAlbums(replyStr, albumsIndex);
		if(!albums.isEmpty()) {
			sortAlbums(&albums);
			albumsModel_->addTopLevelItems(albums);
		}

		int artistsIndex = replyStr.indexOf(QString::fromUtf8("<h1>Поиск по исполнителям</h1>"));
		QList<QompPluginModelItem*> artists;
		if(artistsIndex != -1) {
			QRegExp artistRx(artistsRegExp, Qt::CaseInsensitive);
			artistRx.setMinimal(true);
			while((artistsIndex = artistRx.indexIn(replyStr, artistsIndex)) != -1)
			{
				artistsIndex += artistRx.matchedLength();
				QompPluginArtist* artist = new QompPluginArtist();
				artist->artist = Qomp::unescape(artistRx.cap(3).trimmed());
				artist->internalId = artistRx.cap(2).trimmed();

				//add fake empty items
				bool ok;
				int count = artistRx.cap(4).toInt(&ok);
				QList<QompPluginModelItem*> list;
				if(ok) {
					while(count--)
						list.append(new QompPluginAlbum(artist));
				}
				artist->setItems(list);
				artists.append(artist);
			}
			artistsModel_->addTopLevelItems(artists);
		}
		if(!artists.isEmpty()) {
			dlg_->setCurrentTab(TabArtists);
		}
		else if(!albums.isEmpty()) {
			dlg_->setCurrentTab(TabAlbums);
		}
		else if(!tunes.isEmpty()){
			dlg_->setCurrentTab(TabTracks);
		}
	}
#ifdef DEBUG_OUTPUT
	else
		qDebug() << reply->errorString();
#endif
}

void MyzukaruController::itemSelected(QompPluginModelItem* item)
{
	if(!item || item->internalId.isEmpty())
		return;

	QompPluginTreeModel* model = qobject_cast<QompPluginTreeModel*>(item->model());

	QUrl url(MYZUKA_URL);
	const char* slot = 0;
	switch(item->type()) {
	case QompCon::TypeTune:
	{
		getTuneUrl(item);
		return;
	}
	case QompCon::TypeAlbum:
	{
		QompPluginAlbum *album = static_cast<QompPluginAlbum *>(item);
		if(album->tunesReceived)
			return;
		url.setPath(QUrl::fromPercentEncoding(album->internalId.toLatin1()));
		slot = SLOT(albumUrlFinished());
		break;
	}
	case QompCon::TypeArtist:
	{
		QompPluginArtist *artist = static_cast<QompPluginArtist *>(item);
		if(artist->tunesReceived)
			return;
		url.setPath(QUrl::fromPercentEncoding(QString(artist->internalId + QString("/Albums"))
							.toLatin1()));
		slot = SLOT(artistUrlFinished());
		break;
	}
	default:
		return;
	}
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
#ifndef HAVE_QT6
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif
	QNetworkReply *reply = nam()->get(nr);
	reply->setProperty("id", item->internalId);

	requests_.insert(reply, model);

	connect(reply, SIGNAL(finished()), slot);
	connect(this, &MyzukaruController::destroyed, reply, &QNetworkReply::deleteLater);
	dlg_->startBusyWidget();
}

void MyzukaruController::getSuggestions(const QString &text)
{
#ifndef HAVE_QT6
	static const QRegExp space("\\s+");
#else
	static const QRegularExpression space("\\s+");
#endif
	QUrl url(QString("%1Search/Suggestions?term=%2")
		 .arg(MYZUKA_URL, QString(text).replace(space, "+")), QUrl::StrictMode);
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	nr.setRawHeader("Referer", MYZUKA_URL);
#ifndef HAVE_QT6
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif
	QNetworkReply *reply = nam()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(suggestionsFinished()));
}

void MyzukaruController::suggestionsFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QRegExp sugRx("\"label\":\"([^\"]+)\"");
		sugRx.setMinimal(true);
		int ind = 0;
		QStringList sugs;
		while((ind = sugRx.indexIn(replyStr, ind)) != -1) {
			ind += sugRx.matchedLength();
			sugs.append(Qomp::unescape(sugRx.cap(1)));
		}
		if(!sugs.isEmpty()) {
			emit suggestionsReady(sugs);
		}
	}
}

void MyzukaruController::checkAndStopBusyWidget()
{
	if(requests_.isEmpty())
		dlg_->stopBusyWidget();
}

void MyzukaruController::getTuneUrl(QompPluginModelItem *item)
{
	QompPluginTune* tune = static_cast<QompPluginTune *>(item);
	if(!tune->url.isEmpty())
		return;

	Tune* t = tune->toTune();
	QFutureWatcher<QUrl> *w = new QFutureWatcher<QUrl>(this);
	connect(w, &QFutureWatcher<QUrl>::finished, t, &Tune::deleteLater);
	connect(w, &QFutureWatcher<QUrl>::finished, [w, tune, item]() {
		const QUrl u = w->result();
		w->deleteLater();
		if(!u.isEmpty())
			tune->url = u.toString();

		auto model = static_cast<QompPluginTreeModel*>(item->model());
		model->emitUpdateSignal(model->index(item));
	});

#ifndef HAVE_QT6
	QFuture<QUrl> f = QtConcurrent::run(MyzukaruResolveStrategy::instance(),
					   &MyzukaruResolveStrategy::getBaseUrl, t);
#else
	QFuture<QUrl> f = QtConcurrent::run(&MyzukaruResolveStrategy::getBaseUrl,
					     MyzukaruResolveStrategy::instance(), t);
#endif
	w->setFuture(f);
}

void MyzukaruController::albumUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	QompPluginTreeModel *model = static_cast<QompPluginTreeModel *>(requests_.value(reply));
	requests_.remove(reply);
	checkAndStopBusyWidget();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> tunes = parseTunes2(replyStr, 0);
		if(!tunes.isEmpty()) {
			QString id = reply->property("id").toString();
			QompPluginModelItem* it = model->itemForId(id);
			if(it && it->type() == QompCon::TypeAlbum) {
				QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
				model->setItems(tunes, it);
				foreach(QompPluginModelItem* t, tunes) {
					static_cast<QompPluginTune*>(t)->album = pa->album;
				}
				pa->tunesReceived = true;
			}

		}
	}
}

void MyzukaruController::artistUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	//void* model = requests_.value(reply);
	requests_.remove(reply);
	checkAndStopBusyWidget();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QString id = reply->property("id").toString();
		QompPluginModelItem* it = artistsModel_->itemForId(id);

		//Check if we on first page
		QString urlStr = reply->url().toString();
		int page = 1;
		Q_UNUSED(page)
		QRegExp pageRx("Page(\\d+)");
		if(pageRx.indexIn(urlStr) != -1) {
			page = pageRx.cap(1).toInt();
		}
		else {
			//If we on first page, then clear fake items
			artistsModel_->setItems(QList<QompPluginModelItem*>(), it);
		}

		QList<QompPluginModelItem*> albums = parseAlbums2(replyStr, 0);
		if(!albums.isEmpty()) {
			if(it && it->type() == QompCon::TypeArtist) {
				QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
				foreach(QompPluginModelItem* t, albums) {
					QompPluginAlbum* album = static_cast<QompPluginAlbum*>(t);
					QStringList directAlbums = QStringList() << "2" << "3" << "4" << "6";
					if(directAlbums.contains(album->artist))
						album->artist = pa->artist;
					else
						album->artist.clear();
				}
				pa->tunesReceived = true;
			}
			sortAlbums(&albums);
			artistsModel_->addItems(albums, it);
		}

//		QList<QompPluginModelItem*> tunes = parseTunes3(replyStr, 0);
//		if(!tunes.isEmpty()) {
//			if(it && it->type() == QompCon::TypeArtist) {
//				QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
//				foreach(QompPluginModelItem* t, tunes) {
//					static_cast<QompPluginTune*>(t)->artist = pa->artist;
//				}
//				pa->tunesReceived = true;
//			}
//			artistsModel_->addItems(tunes, it);
//		}

//		int maxPage = page;
//		QRegExp maxPageRx("<a href=\"/Artist/.+/Page[\\d]+\">([\\d]+)</a>");
//		maxPageRx.setMinimal(true);
//		if(maxPageRx.lastIndexIn(replyStr) != -1) {
//			maxPage = maxPageRx.cap(1).toInt();
//		}
		//Take Next page
//		if(page < maxPage) {
//			QUrl url(MYZUKA_URL);
//			url.setPath(QString("%1/Page%2").arg(QUrl::fromPercentEncoding(it->internalId.toLatin1()), QString::number(++page)));
//			QNetworkRequest nr(url);
//			nr.setRawHeader("Accept", "*/*");
//			nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
//			QNetworkReply *newReply = nam()->get(nr);
//			newReply->setProperty("id", id);
//			requests_.insert(newReply, model);
//			connect(newReply, SIGNAL(finished()), SLOT(artistUrlFinished()));
//			dlg_->startBusyWidget();
//		}
	}
}
