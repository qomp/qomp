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

#include "yandexmusicgettunsdlg.h"
#include "qompplugintreeview.h"
#include "qompplugintreemodel.h"
#include "qompplugintypes.h"
#include "common.h"
#include "yandexmusicurlresolvestrategy.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
//#include <QDebug>



// cap(1) - song title
// cap(2) - song id
// cap(3) - storage dir
// cap(4) - artist
// cap(5) - album
// cap(6) - song duration
// cap(7) - cover
static const QString trackReString = "<div class=\"b-track .*title=\"([^\"]+)\" .*onclick=[\'\"]return "
		"\\{\"id\":\"([^\"]*)\", \"storage_dir\":\"([^\"]*)\", \"title\":\"[^\"]*\", \"artist\":\"([^\"]*)\", \"artist_id\":\"[^\"]*\","
		" \"artist_var\":\"[^\"]*\", \"album\":\"([^\"]*)\", \"album_id\":\"[^\"]*\", \"duration\":\"([^\"]*)\","
		" \"cover\":\"([^\"]*)\"\\}";


// cap(1) - album title
// cap(2) - album id
// cap(3) - tracks count
static const QString albumReString = "<div class=\"b-albums\" title=\"([^\"]+)\">.+"
		"onclick=[\'\"]return \\{'id':\"([^&]*)\", 'title':\"[^&]*\", 'track_count':([^,]+),";


// cap(1) - artist id
// cap(2) - artist
// cap(3) - artist mark
static const QString artistReString = "<div class=\"b-artist-group \"><a href=\"([^\"]+)\" class=\"b-link\"><span class=\"b-mark\">([^<]+)</span>([^<]*)</a>";


static const QString currentPageReString = "<b class=\"b-pager__current\">([^<]*)</b>";


// cap(1) - serch text
// cap(2) - page number
static const QString pageReString = "<a class=\"b-pager__page\" href=\"#!/search\\?text=([^&]+)&[^\"]+\">(\\d*)</a>";



class YandexMusicTune : public QompPluginTune
{
public:
	YandexMusicTune(QompPluginModelItem* parent = 0) :
		QompPluginTune(parent) {}

	QString durationToString() const
	{
		return Qomp::durationMiliSecondsToString(duration.toLongLong());
	}

	virtual Tune toTune() const
	{
		Tune t = QompPluginTune::toTune();
		t.duration = durationToString();
		return t;
	}
};



YandexMusicGettunsDlg::YandexMusicGettunsDlg() :
	QompPluginGettunesDlg(),
	tracksModel_(new QompPluginTreeModel(this)),
	albumsModel_(new QompPluginTreeModel(this)),
	artistsModel_(new QompPluginTreeModel(this)),
	tabWidget_(new QTabWidget(this))
{
	setWindowTitle("Yandex.Music");

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
}

void YandexMusicGettunsDlg::accept()
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

		Tune tune_ = pt->toTune();
		tune_.setUrlResolveStrategy(YandexMusicURLResolveStrategy::instance());
		tunes_.append(tune_);
	}

	QDialog::accept();
}

void YandexMusicGettunsDlg::doSearch()
{
	const QString txt = currentSearchText();
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
	req.insert("artists", SLOT(artistsSearchFinished()));
	req.insert("albums", SLOT(albumsSearchFinished()));
	req.insert("tracks", SLOT(tracksSearchFinished()));
	foreach(const QString& key, req.keys()) {
		search(txt, key, req.value(key));
	}
}

void YandexMusicGettunsDlg::search(const QString& text, const QString &type, const char *slot, int page)
{
	static const QString urlStr = "http://music.yandex.ru/fragment/search?text=%1&&type=%2";
	QString url = urlStr.arg(text, type);
	if(page > 1) {
		url += "&page=" + QString::number(page);
	}
	QNetworkRequest nr(url);
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), slot);
	requests_.insert(reply, 0);
	startBusyWidget();
}

void YandexMusicGettunsDlg::searchNextPage(const QString &reply, const QString &type, const char *slot)
{
	QRegExp currentPageRe(currentPageReString);
	if(currentPageRe.indexIn(reply) != -1) {
		int curPage = currentPageRe.cap(1).toInt();
		QRegExp pageRe(pageReString);
		if(pageRe.lastIndexIn(reply) != -1) {
			int lastPage = pageRe.cap(2).toInt();
			if(lastPage > curPage) {
				search(pageRe.cap(1), type, slot, ++curPage);
			}
		}
	}
}

void YandexMusicGettunsDlg::artistsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QRegExp artistRe(artistReString);
		artistRe.setMinimal(true);
		int off = 0;
		QList<QompPluginModelItem*> artists;
		while((off = artistRe.indexIn(replyStr, off)) != -1) {
			off += artistRe.matchedLength();
			QompPluginArtist* artist = new QompPluginArtist;
			artist->artist = Qomp::unescape(artistRe.cap(2)+artistRe.cap(3));
			artist->internalId = artistRe.cap(1).replace("#!", "fragment");
			artist->setItems(QList<QompPluginModelItem*>() << new QompPluginTune()); //add fake item
			artists.append(artist);
		}
		artistsModel_->addTopLevelItems(artists);

		searchNextPage(replyStr, "artists", SLOT(artistsSearchFinished()));
	}
}

static QList<QompPluginModelItem*> parseAlbums(const QString& str)
{
	QRegExp albumRe(albumReString);
	albumRe.setMinimal(true);
	QString string = str;
	string.replace("&quot;", "\"");
	int off = 0;
	QList<QompPluginModelItem*> albums;
	while((off = albumRe.indexIn(string, off)) != -1) {
		off += albumRe.matchedLength();
		QompPluginAlbum* album = new QompPluginAlbum;
		album->album = albumRe.cap(1);
		album->internalId = albumRe.cap(2);
		bool ok = false;
		int count = albumRe.cap(3).toInt(&ok);
		QList<QompPluginModelItem*> fakeItems;
		do {
			fakeItems << new QompPluginTune(); //add fake item
		} while(ok && --count > 1);

		album->setItems(fakeItems);
		albums.append(album);
	}
	return albums;
}

static QList<QompPluginModelItem*> parseTunes(const QString& str)
{
	QRegExp tuneRe(trackReString);
	tuneRe.setMinimal(true);
	QString string = str;
	string.replace("&quot;", "\"");
	int off = 0;
	QList<QompPluginModelItem*> tracks;
	while((off = tuneRe.indexIn(string, off)) != -1) {
		off += tuneRe.matchedLength();
		YandexMusicTune* tune = new YandexMusicTune;
		tune->title = Qomp::unescape(tuneRe.cap(1));
		tune->artist = Qomp::unescape(tuneRe.cap(4));
		tune->album = Qomp::unescape(tuneRe.cap(5));
		tune->duration = tuneRe.cap(6);
		tune->internalId = tuneRe.cap(2);
		tune->url = tuneRe.cap(3);
		tracks.append(tune);
	}
	return tracks;
}

void YandexMusicGettunsDlg::albumsSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		const QString replyStr = QString::fromUtf8(reply->readAll());
		albumsModel_->addTopLevelItems(parseAlbums(replyStr));

		searchNextPage(replyStr, "albums", SLOT(albumsSearchFinished()));
	}
}

void YandexMusicGettunsDlg::tracksSearchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		const QString replyStr = QString::fromUtf8(reply->readAll());
		tracksModel_->addTopLevelItems(parseTunes(replyStr));

		searchNextPage(replyStr, "tracks", SLOT(tracksSearchFinished()));
	}
}

void YandexMusicGettunsDlg::albumUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	void* model = requests_.value(reply);
	requests_.remove(reply);
	checkAndStopBusyWidget();

	if(reply->error() == QNetworkReply::NoError) {
		const QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> list = parseTunes(replyStr);
		if(!list.isEmpty()) {
			QompPluginTreeModel *model_ = (QompPluginTreeModel *)model;
			QompPluginModelItem* it = model_->itemForId(reply->property("id").toString());
			model_->setItems(list, it);

			QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
			pa->tunesReceived = true;
		}
	}
}

void YandexMusicGettunsDlg::artistUrlFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);
	checkAndStopBusyWidget();
	if(reply->error() == QNetworkReply::NoError) {
		const QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> list = parseAlbums(replyStr);
		if(!list.isEmpty()) {
			QompPluginModelItem* it = artistsModel_->itemForId(reply->property("id").toString());
			artistsModel_->setItems(list, it);
			QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
			pa->tunesReceived = true;
		}
	}
}

void YandexMusicGettunsDlg::itemSelected(const QModelIndex &ind)
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
		path = QUrl::fromPercentEncoding("fragment/album/" + album->internalId.toLatin1());
		slot = SLOT(albumUrlFinished());
		break;
	}
	case Qomp::TypeArtist:
	{
		QompPluginArtist *artist = static_cast<QompPluginArtist *>(item);
		if(artist->tunesReceived)
			return;
		path = QUrl::fromPercentEncoding(QString("%1/albums").arg(item->internalId).toLatin1());
		slot = SLOT(artistUrlFinished());
		break;
	}
	default:
		return;
	}

	QUrl url("http://music.yandex.ru/" + path, QUrl::StrictMode);
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = nam_->get(nr);
	reply->setProperty("id", item->internalId);

	requests_.insert(reply, model);
	connect(reply, SIGNAL(finished()), slot);
	startBusyWidget();
}

void YandexMusicGettunsDlg::getSuggestions(const QString &text)
{
	static const QString urlStr = "http://suggest-music.yandex.ru/suggest-ya.cgi?v=3&part=%1";
	QNetworkRequest nr(urlStr.arg(text));
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(suggestionsFinished()));
}

void YandexMusicGettunsDlg::suggestionsFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QRegExp sugRx("\\[([^\\]]+)\\]");
		if(sugRx.indexIn(replyStr) != -1 ) {
			QStringList sugs = sugRx.cap(1).split(",");
			if(!sugs.isEmpty()) {
				for(QStringList::Iterator it = sugs.begin(); it != sugs.end(); ++ it) {
					*it = (*it).mid(1, (*it).size() - 2);
				}
				newSuggestions(sugs);
			}
		}
	}
}

void YandexMusicGettunsDlg::checkAndStopBusyWidget()
{
	if(requests_.isEmpty())
		stopBusyWidget();
}
