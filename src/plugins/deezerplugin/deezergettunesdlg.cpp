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

#include "deezergettunesdlg.h"
#include "qompplugintreeview.h"
#include "common.h"
#include "qompplugintreemodel.h"
#include "qompplugintypes.h"
#include "deezerplugindefines.h"
#include "deezerauth.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>
#include <QSet>

//#include <QtDebug>

DeezerGettunesDlg::DeezerGettunesDlg(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	tracksModel_(new QompPluginTreeModel(this)),
	albumsModel_(new QompPluginTreeModel(this)),
	artistsModel_(new QompPluginTreeModel(this)),
	tabWidget_(new QTabWidget(this))
{
	setWindowTitle(DEEZER_PLUGIN_NAME);

	QompPluginTreeView* tracksView = new QompPluginTreeView(this);
	QompPluginTreeView* albumsView = new QompPluginTreeView(this);
	QompPluginTreeView* artistsView = new QompPluginTreeView(this);

	tracksView->setModel(tracksModel_);
	albumsView->setModel(albumsModel_);
	artistsView->setModel(artistsModel_);

	tabWidget_->addTab(artistsView, tr("Artists"));
	tabWidget_->addTab(albumsView, tr("Albums"));
	tabWidget_->addTab(tracksView, tr("Tracks"));

	setResultsWidget(tabWidget_);

	QList<QompPluginTreeView*> list = QList<QompPluginTreeView*> () << artistsView << albumsView << tracksView;
	foreach(QompPluginTreeView* view, list) {
		connect(view, SIGNAL(clicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
		connect(view, SIGNAL(expanded(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	}
	connect(this, SIGNAL(searchTextChanged(QString)), SLOT(getSuggestions(QString)));
//	DeezerAuth* a = new DeezerAuth(this);
//	a->start();
}

void DeezerGettunesDlg::accept()
{
	QList<QompPluginModelItem*> list = QList<QompPluginModelItem*>()
			<< artistsModel_->selectedItems()
			<< albumsModel_->selectedItems()
			<< tracksModel_->selectedItems();

	foreach(QompPluginModelItem* tune, list) {
		if(!tune || tune->type() != Qomp::TypeTune)
			continue;

		QompPluginTune* pt = static_cast<QompPluginTune*>(tune);
		if(pt->url.isEmpty())
			continue;

		tunes_.append(pt->toTune());
	}

	QDialog::accept();
}

void DeezerGettunesDlg::doSearch()
{
	const QString txt = currentSearchText();
	if(txt.isEmpty())
		return;

	foreach(QNetworkReply* r, requests_.keys()) {
		r->deleteLater();
	}
	requests_.clear();

	albumsModel_->reset();
	tracksModel_->reset();
	artistsModel_->reset();

	QHash<QString, const char*> req;
	req.insert("artist", SLOT(artistsSearchFinished()));
	req.insert("album", SLOT(albumsSearchFinished()));
	req.insert("track", SLOT(tracksSearchFinished()));
	foreach(const QString& key, req.keys()) {
		search(txt, key, req.value(key));
	}
}

void DeezerGettunesDlg::artistsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QDomDocument doc;
		if(doc.setContent(replyStr)) {
			QDomElement root = doc.documentElement();
			QDomElement artistElem = root.firstChildElement("data").firstChildElement("artist");
			QList<QompPluginModelItem*> artists;
			while(!artistElem.isNull()) {
				QompPluginArtist* artist = new QompPluginArtist;
				artist->artist = Qomp::unescape(artistElem.firstChildElement("name").text());
				artist->internalId = artistElem.firstChildElement("id").text();
				//add fake empty items
				bool ok;
				int count = artistElem.firstChildElement("nb_album").text().toInt(&ok);
				if(count == 0) {
					delete artist;
				}
				else {
					QList<QompPluginModelItem*> list;
					if(ok) {
						while(count--)
							list.append(new QompPluginTune(artist));
					}
					artist->setItems(list);
					artists.append(artist);
				}
				artistElem = artistElem.nextSiblingElement("artist");
			}
			if(!artists.isEmpty()) {
				artistsModel_->addTopLevelItems(artists);
				searchNextPage(replyStr, "artist", SLOT(artistsSearchFinished()));
			}
		}

	}
}

static QList<QompPluginModelItem*> parseAlbums(const QString& replyStr)
{
	QList<QompPluginModelItem*> albums;
	QDomDocument doc;
	if(doc.setContent(replyStr)) {
		QDomElement root = doc.documentElement();
		QDomElement albumElem = root.firstChildElement("data").firstChildElement("album");
		while(!albumElem.isNull()) {
			QompPluginAlbum* album = new QompPluginAlbum;
			album->artist = Qomp::unescape(albumElem.firstChildElement("artist").firstChildElement("name").text());
			album->album = Qomp::unescape(albumElem.firstChildElement("title").text());
			album->internalId = albumElem.firstChildElement("id").text();
			album->year = albumElem.firstChildElement("release_date").text();
			//add fake empty items
			bool ok;
			int count = albumElem.firstChildElement("nb_tracks").text().toInt(&ok);
			QList<QompPluginModelItem*> list;
			if(ok) {
				do
					list.append(new QompPluginTune(album));
				while(--count > 0);
			}
			if(list.isEmpty())
				list.append(new QompPluginTune(album));
			album->setItems(list);
			albums.append(album);
			albumElem = albumElem.nextSiblingElement("album");
		}
	}
	return albums;
}

void DeezerGettunesDlg::albumsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> albums = parseAlbums(replyStr);
		if(!albums.isEmpty()) {
			albumsModel_->addTopLevelItems(albums);
			searchNextPage(replyStr, "album", SLOT(albumsSearchFinished()));
		}

	}
}

static QList<QompPluginModelItem*> parseTunes(const QString& replyStr)
{
	QList<QompPluginModelItem*> tunes;
	QDomDocument doc;
	if(doc.setContent(replyStr)) {
		QDomElement root = doc.documentElement();
		QDomElement tuneElem = root.firstChildElement("data").firstChildElement("track");
		if(tuneElem.isNull())
			tuneElem = root.firstChildElement("tracks").firstChildElement("data").firstChildElement("track");
		while(!tuneElem.isNull()) {
			QompPluginTune* tune = new QompPluginTune;
			tune->artist = Qomp::unescape(tuneElem.firstChildElement("artist").firstChildElement("name").text());
			tune->album = Qomp::unescape(tuneElem.firstChildElement("album").firstChildElement("title").text());
			tune->title = Qomp::unescape(tuneElem.firstChildElement("title").text());
			tune->internalId = tuneElem.firstChildElement("id").text();
			tune->url = tuneElem.firstChildElement("preview").text();
			tunes.append(tune);
			tuneElem = tuneElem.nextSiblingElement("track");
		}
	}
	return tunes;
}

void DeezerGettunesDlg::tracksSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> tracks = parseTunes(replyStr);
		if(!tracks.isEmpty()) {
			tracksModel_->addTopLevelItems(tracks);
			searchNextPage(replyStr, "track", SLOT(tracksSearchFinished()));
		}

	}
}

void DeezerGettunesDlg::tuneUrlFinished()
{
}

void DeezerGettunesDlg::albumUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	void* model = requests_.value(reply);
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> tracks = parseTunes(replyStr);
		if(!tracks.isEmpty()) {
			QompPluginTreeModel *model_ = (QompPluginTreeModel *)model;
			QompPluginModelItem* it = model_->itemForId(reply->property("id").toString());
			model_->setItems(tracks, it);

			QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
			pa->tunesReceived = true;
		}

	}
}

void DeezerGettunesDlg::artistUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> albums = parseAlbums(replyStr);
		if(!albums.isEmpty()) {
			QompPluginModelItem* it = artistsModel_->itemForId(reply->property("id").toString());
			artistsModel_->setItems(albums, it);
			QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
			pa->tunesReceived = true;
		}

	}
}

void DeezerGettunesDlg::itemSelected(const QModelIndex &ind)
{
	QAbstractItemView* view = qobject_cast<QAbstractItemView*>(sender());
	if(!view)
		return;

	QompPluginTreeModel* model = qobject_cast<QompPluginTreeModel*>(view->model());
	QompPluginModelItem* item = model->item(ind);

	if(!item || item->internalId.isEmpty())
		return;

	QString path;
	const char* slot = 0;
	switch(item->type()) {
	case Qomp::TypeTune:
	{
//		YandexMusicTune *tune = static_cast<YandexMusicTune *>(item);
//		if(!tune->url.isEmpty())
//			return;
//		url = QUrl(QString("http://storage.music.yandex.ru/get/%1/2.xml").arg(tune->storageDir), QUrl::StrictMode);
//		slot = SLOT(tuneUrlFinishedStepOne());
		return;
	}
	case Qomp::TypeAlbum:
	{
		QompPluginAlbum *album = static_cast<QompPluginAlbum *>(item);
		if(album->tunesReceived)
			return;
		path = QString("album/%1?output=xml").arg(album->internalId);
		slot = SLOT(albumUrlFinished());
		break;
	}
	case Qomp::TypeArtist:
	{
		QompPluginArtist *artist = static_cast<QompPluginArtist *>(item);
		if(artist->tunesReceived)
			return;
		path = QString("artist/%1/albums?output=xml").arg(artist->internalId);
		slot = SLOT(artistUrlFinished());
		break;
	}
	default:
		return;
	}

	QUrl url(DEEZER_URL + path + DEEZER_APP_ID, QUrl::StrictMode);
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = nam_->get(nr);
	reply->setProperty("id", item->internalId);

	requests_.insert(reply, model);
	connect(reply, SIGNAL(finished()), slot);
	startBusyWidget();
}


void DeezerGettunesDlg::checkAndStopBusyWidget()
{
	if(requests_.isEmpty())
		stopBusyWidget();
}

void DeezerGettunesDlg::search(const QString &text, const QString &type, const char *slot, int page)
{
	QString url = QString("%1/search/%2/?q=%3&nb_items=20&output=xml").arg(DEEZER_URL, type, text);
	if(page > 1) {
		url += "&index=" + QString::number(page);
	}
	if(!userToken_.isEmpty())
		url += "&access_token="+userToken_;
	url += DEEZER_APP_ID;

	QNetworkRequest nr(url);
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), slot);
	requests_.insert(reply, 0);
	startBusyWidget();
}

void DeezerGettunesDlg::searchNextPage(const QString &reply, const QString &type, const char *slot)
{
	QDomDocument doc;
	if(doc.setContent(reply)) {
		QDomElement root = doc.documentElement();
		QDomElement next = root.firstChildElement("next");
		if(!next.isNull()) {
			QRegExp n("\\?q=([^&]+)&\\S+&index=(\\d+)");
			if(n.indexIn(next.text()) != -1) {
				search(n.cap(1),type, slot, n.cap(2).toInt());
			}
		}
	}
}


void DeezerGettunesDlg::getSuggestions(const QString &text)
{
	QString url = QString("http://www.deezer.com/en/suggest?q=%1&limit=4").arg(text);

	QNetworkRequest nr(url);
	nr.setRawHeader("X-Requested-With", "MLHttpRequest");
	nr.setRawHeader("Accept", "*/*");
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(suggestionsFinished()));
}

void DeezerGettunesDlg::suggestionsFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();

	if(reply->error() == QNetworkReply::NoError) {
		const QString res = QString::fromUtf8(reply->readAll());
		QRegExp art("\"ART_NAME\":\"([^\"]+)\"");
		int index = 0;
		QSet<QString> sugs;
		while( (index = art.indexIn(res, index)) != -1 ) {
			index += art.matchedLength();
			sugs.insert(art.cap(1));
		}
		index = 0;
		QRegExp alb("\"ALB_TITLE\":\"([^\"]+)\"");
		while( (index = alb.indexIn(res, index)) != -1 ) {
			index += alb.matchedLength();
			sugs.insert(alb.cap(1));
		}
		index = 0;
		QRegExp track("\"SNG_TITLE\":\"([^\"]+)\"");
		while( (index = track.indexIn(res, index)) != -1 ) {
			index += track.matchedLength();
			sugs.insert(track.cap(1));
		}

		if(!sugs.isEmpty())
				newSuggestions(sugs.toList());

	}
}
