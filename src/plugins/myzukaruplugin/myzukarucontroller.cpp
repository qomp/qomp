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

#include "myzukarucontroller.h"
#include "myzukarugettunesdlg.h"
#include "qompplugintypes.h"
#include "common.h"
#include "myzukarumodels.h"
#include "myzukarudefines.h"
#include "myzukaruresolvestrategy.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>


static const QString artistsRegExp = QString(
		"<tr>\\s+"
		"<td .+"
		"</td>\\s+"
		"<td>\\s+"
		"<a (Class=\"darkorange\")? href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+"		//cap(2) - internalID, cap(3) - Artist
		"</td>\\s+"
		"<td>\\s+"
		"(\\d+)\\s+"									//cap(4) - albums count
		"</td>\\s+</tr>");

static const QString albumsRegExp = QString(
		"<tr>\\s+"
		"<td .+"
		"</td>\\s+"
		"<td>\\s+"
		"<a (Class=\"darkorange\")? href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+"		//cap(3) - Artist
		"</td>\\s+"
		"<td>\\s+"
		"<a (class=\"darkorange\")? href=\"(/Album/[^\"]+)\">"				//cap(5) - internalId
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

static const QString albumsRegExp2 = QString(
			"<div itemtype=.+>\\s+"
			"<div>\\s+"
			"<a.+href=\"(/Album/[^\"]+)\">\\s+"					//cap(1) - internalId
			"([^<]+)"								//cap(2) - album
			"</a>.+"
			"<a .+"
			"</a>\\s+"
			"</div>.+"
			"Год релиза: (\\d+)?.+"							//cap(3) - year
			"</div>"
			);

static const QString songsRegExp = QString(
		"<tr>\\s+"
		"<td .+"
		"<a (Class=\"darkorange\")? href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+"		//cap(3) - artist
		"</td>\\s+"
		"<td>\\s+"
		"<a (Class=\"darkorange\")? href=\"/Song/([^/]+)/[^\"]+\">([^<]+)</a>\\s+"	//cap(5) - internalID, cap(6) - Title
		"</td>\\s+"
		"<td>\\s+"
		"([^<]+)\\s+"									//cap(7) - size
		"</td>\\s+</tr>");

static const QString songsRegExp2 = QString(
		"<tr id=\"trSong_[^>]+>\\s+"
		"<td .+"
		"</td>\\s+"
		"<td class=\"tdNum\">\\s+"
		"(\\d+)\\s+"									//cap(1) - track number
		"</td>\\s+"
		"<td .+"
		"<div>\\s+"
		"<a href=\"/Artist/[^\"]+\">([^<]+)</a>.+"					//cap(2) - artist
		"<a href=\"/Song/([^/]+)/[^\"]+\">([^<]+)</a>.+"				//cap(3) - internalId, cap(4) - title
		"</div>\\s+"
		"</td>\\s+"
		"<td class=\"tdOst\">\\s+"
		"(\\S+)\\s+"									//cap(5) - duration
		"</td>"
		);


static const QString songsRegExp3 = QString(
		"<tr id=\"trSong_[^>]+>\\s+"
		"<td .+"
		"</td>\\s+"
		"<td class=\"tdNum\">\\s+"
		"</td>\\s+"
		"<td .+"
		"<div>\\s+"
		"<a href=\"/Song/([^/]+)/[^\"]+\">([^<]+)</a>.+"				//cap(1) - internalId, cap(2) - title
		"</div>\\s+"
		"</td>\\s+"
		"<td .+"
		"<a href=\"/(Album/[^\"]+)\">([^<]+)</a>.+"					//cap(4) - album
		"<td class=\"tdOst\">\\s+"
		"(\\S+)\\s+"									//cap(5) - duration
		"</td>"
		);


static QList<QompPluginModelItem*> parseTunes(const QString& replyStr, int songIndex)
{
	QList<QompPluginModelItem*> tunes;
	if(songIndex != -1) {
		QRegExp songRx(songsRegExp, Qt::CaseInsensitive);
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QompPluginTune* t = new QompPluginTune();
			t->/*internalId*/url = songRx.cap(5);
			t->artist = Qomp::unescape(songRx.cap(3));
			t->title = Qomp::unescape(songRx.cap(6));
			//t->album = unescape(songRx.cap(9));

			tunes.append(t);
		}
	}
	return tunes;
}

static QList<QompPluginModelItem*> parseTunes2(const QString& replyStr, int songIndex)
{
	QList<QompPluginModelItem*> tunes;
	if(songIndex != -1) {
		QRegExp songRx(songsRegExp2, Qt::CaseInsensitive);
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QompPluginTune* t = new QompPluginTune();
			t->/*internalId*/url = songRx.cap(3);
			t->artist = Qomp::unescape(songRx.cap(2));
			t->title = Qomp::unescape(songRx.cap(4));
			t->duration = Qomp::unescape(songRx.cap(5));

			tunes.append(t);
		}
	}
	return tunes;
}

static QList<QompPluginModelItem*> parseTunes3(const QString& replyStr, int songIndex)
{
	QList<QompPluginModelItem*> tunes;
	if(songIndex != -1) {
		QRegExp songRx(songsRegExp3, Qt::CaseInsensitive);
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QompPluginTune* t = new QompPluginTune();
			t->/*internalId*/url = songRx.cap(1);
			t->album = Qomp::unescape(songRx.cap(4));
			t->title = Qomp::unescape(songRx.cap(2));
			t->duration = Qomp::unescape(songRx.cap(5));

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
	QList<QompPluginModelItem*> albums;
	if(albumsIndex != -1) {
		QRegExp albumRx(albumsRegExp2, Qt::CaseInsensitive);
		albumRx.setMinimal(true);
		while((albumsIndex = albumRx.indexIn(replyStr, albumsIndex)) != -1) {
			albumsIndex += albumRx.matchedLength();
			QompPluginAlbum* album = new QompPluginAlbum();
			//album->artist = Qomp::unescape(albumRx.cap(3).trimmed());
			album->album = Qomp::unescape(albumRx.cap(2).trimmed());
			album->year = albumRx.cap(3).trimmed();
			album->internalId = albumRx.cap(1).trimmed();

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
	artistsModel_(new MyzukaruArtistsModel(this)),
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

	const QString url = QString("%1/Search?searchText=%2").arg(MYZUKA_URL,txt);
	QNetworkRequest nr;
	nr.setUrl(url);
	QNetworkReply* reply = nam()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
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

		int songIndex = replyStr.indexOf("<a name=\"songs\"></a>");
		QList<QompPluginModelItem*> tunes = parseTunes(replyStr, songIndex);
		if(!tunes.isEmpty()) {
			tracksModel_->addTopLevelItems(tunes);
		}

		int albumsIndex = replyStr.indexOf("<a name=\"albums\"></a>");
		QList<QompPluginModelItem*> albums = parseAlbums(replyStr, albumsIndex);
		if(!albums.isEmpty()) {
			albumsModel_->addTopLevelItems(albums);
		}

		int artistsIndex = replyStr.indexOf("<a name=\"artists\"></a>");
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
		url.setPath(QUrl::fromPercentEncoding(artist->internalId.toLatin1()));
		slot = SLOT(artistUrlFinished());
		break;
	}
	default:
		return;
	}
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = nam()->get(nr);
	reply->setProperty("id", item->internalId);

	requests_.insert(reply, model);

	connect(reply, SIGNAL(finished()), slot);
	dlg_->startBusyWidget();
}

void MyzukaruController::getSuggestions(const QString &text)
{
	QUrl url(QString("%1/Search/Suggestions?term=%2")
		 .arg(MYZUKA_URL,text), QUrl::StrictMode);
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "application/json, text/javascript, */*; q=0.01");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	nr.setRawHeader("Referer", MYZUKA_URL);
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

void MyzukaruController::albumUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	QompPluginTreeModel *model = (QompPluginTreeModel *)requests_.value(reply);
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
	void* model = requests_.value(reply);
	requests_.remove(reply);
	checkAndStopBusyWidget();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QString id = reply->property("id").toString();
		QompPluginModelItem* it = artistsModel_->itemForId(id);

		//Check if we on first page
		QString urlStr = reply->url().toString();
		int page = 1;
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
					static_cast<QompPluginAlbum*>(t)->artist = pa->artist;
				}
				pa->tunesReceived = true;
			}
			artistsModel_->addItems(albums, it);
		}

		QList<QompPluginModelItem*> tunes = parseTunes3(replyStr, 0);
		if(!tunes.isEmpty()) {
			if(it && it->type() == QompCon::TypeArtist) {
				QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
				foreach(QompPluginModelItem* t, tunes) {
					static_cast<QompPluginTune*>(t)->artist = pa->artist;
				}
				pa->tunesReceived = true;
			}
			artistsModel_->addItems(tunes, it);
		}

		int maxPage = page;
		QRegExp maxPageRx("<a href=\"/Artist/.+/Page[\\d]+\">([\\d]+)</a>");
		maxPageRx.setMinimal(true);
		if(maxPageRx.lastIndexIn(replyStr) != -1) {
			maxPage = maxPageRx.cap(1).toInt();
		}
		//Take Next page
		if(page < maxPage) {
			QUrl url(MYZUKA_URL);
			url.setPath(QString("%1/Page%2").arg(QUrl::fromPercentEncoding(it->internalId.toLatin1()), QString::number(++page)));
			QNetworkRequest nr(url);
			nr.setRawHeader("Accept", "*/*");
			nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
			QNetworkReply *newReply = nam()->get(nr);
			newReply->setProperty("id", id);
			requests_.insert(newReply, model);
			connect(newReply, SIGNAL(finished()), SLOT(artistUrlFinished()));
			dlg_->startBusyWidget();
		}
	}
}
