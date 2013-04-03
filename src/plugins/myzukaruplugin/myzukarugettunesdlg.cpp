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

#include "myzukarugettunesdlg.h"
#include "myzukarumodels.h"
#include "qompplugintracksview.h"
#include "qompplugintreeview.h"
#include "qompplugintracksmodel.h"
#include "common.h"
#include "myzukarudefines.h"
#include "qompplugintypes.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>


MyzukaruGettunesDlg::MyzukaruGettunesDlg(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	tracksModel_(new QompPluginTracksModel(this)),
	albumsModel_(new QompPluginTreeModel(this)),
	artistsModel_(new MyzukaruArtistsModel(this))
{
	setWindowTitle(MYZUKA_PLUGIN_NAME);

	QompPluginTracksView* tracksView = new QompPluginTracksView(this);
	QompPluginTreeView* albumsView = new QompPluginTreeView(this);
	QompPluginTreeView* artistsView = new QompPluginTreeView(this);

	tracksView->setModel(tracksModel_);
	albumsView->setModel(albumsModel_);
	artistsView->setModel(artistsModel_);

	QTabWidget* tabWidget = new QTabWidget(this);
	tabWidget->addTab(artistsView, tr("Artists"));
	tabWidget->addTab(albumsView, tr("Albums"));
	tabWidget->addTab(tracksView, tr("Tracks"));

	setResultsWidget(tabWidget);

	connect(tracksView, SIGNAL(tuneSelected(QompPluginModelItem*)), SLOT(itemSelected(QompPluginModelItem*)));
	connect(albumsView, SIGNAL(itemSelected(QompPluginModelItem*)), SLOT(itemSelected(QompPluginModelItem*)));
	connect(artistsView, SIGNAL(itemSelected(QompPluginModelItem*)), SLOT(itemSelected(QompPluginModelItem*)));
}

void MyzukaruGettunesDlg::accept()
{
	QList<QompPluginModelItem*> list(tracksModel_->selectedTunes());
	list << albumsModel_->selectedItems() << artistsModel_->selectedItems();

	foreach(QompPluginModelItem* t, list) {
		addTune(t);
	}
	QDialog::accept();
}

void MyzukaruGettunesDlg::addTune(QompPluginModelItem *tune)
{
	if(!tune || tune->type() != Qomp::TypeTune)
		return;

	QompPluginTune* pt = static_cast<QompPluginTune*>(tune);
	if(pt->url.isEmpty())
		return;

	Tune t;
	t.artist = pt->artist;
	t.title = pt->title;
	t.album == pt->album;
	t.url = pt->url;
	tunes_.append(t);
}

void MyzukaruGettunesDlg::doSearch()
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

	const QString url = QString("http://myzuka.ru/Search?searchText=%1").arg(txt);
	QNetworkRequest nr;
	nr.setUrl(url);
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
	requests_.insert(reply, 0);
}

static QList<QompPluginModelItem*> parseTunes(const QString& replyStr, int songIndex)
{
	QList<QompPluginModelItem*> tunes;
	if(songIndex != -1) {
		QRegExp songRx("(<a\\s*(Class=\"darkorange\"|class=\"link_orange\")?\\s*href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+</td>\\s+)?"		//Link to Artist
			       "<td( height=[^>]+)?>\\s+<a\\s*(Class=\"darkorange\"|class=\"link_orange\")?\\s*href=\"(/Song/[^\"]+)\">([^<]+)</a>"	//Link to Song
			       /*"(\\s+</td>\\s+<td>([^<]+)</td>)?"*/, Qt::CaseInsensitive);								//Album name
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QRegExp idRx("/(\\d+)/");
			if(idRx.indexIn(songRx.cap(7)) != -1) {
				QompPluginTune* t = new QompPluginTune();
				t->internalId = idRx.cap(1);
				t->artist = unescape(songRx.cap(4));
				t->title = unescape(songRx.cap(8));
				//t->album = unescape(songRx.cap(9));

				tunes.append(t);
			}
		}
	}
	return tunes;
}

static QList<QompPluginModelItem*> parseAlbums(const QString& replyStr, int albumsIndex)
{
	QList<QompPluginModelItem*> albums;
	if(albumsIndex != -1) {
		QRegExp albumRx("(<a(\\s+Class=\"darkorange\"|\\s+class=\"link_orange\")?\\s+href=\"(/Artist/[^\"]+)\">([^<]+)</a>\\s+</td>\\s+<td>\\s+)?"
				"<a(\\s+Class=\"darkorange\"|\\s+class=\"link_orange\")?\\s+href=\"(/Album/[^\"]+)\">([^<]+)</a>\\s+</td>\\s+<td>\\s+(\\d+)\\s+"
				"</td>\\s+<td>\\s+(\\d+)\\s+</td>", Qt::CaseInsensitive);
		albumRx.setMinimal(true);
		while((albumsIndex = albumRx.indexIn(replyStr, albumsIndex)) != -1) {
			albumsIndex += albumRx.matchedLength();
			QompPluginAlbum* album = new QompPluginAlbum();
			album->artist = unescape(albumRx.cap(4).trimmed());
			album->album = unescape(albumRx.cap(7).trimmed());
			album->year = albumRx.cap(8);
			album->internalId = albumRx.cap(6).trimmed();
			bool ok;
			int count = albumRx.cap(9).toInt(&ok);
			QList<QompPluginModelItem*> list;
			if(ok) {
				while(count) {
					list.append(new QompPluginTune(album));
					--count;
				}
			}
			album->setItems(list);
			albums.append(album);
		}
	}
	return albums;
}

void MyzukaruGettunesDlg::searchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	requests_.remove(reply);

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());

		int songIndex = replyStr.indexOf("<a name=\"songs\"></a>");
		QList<QompPluginModelItem*> tunes = parseTunes(replyStr, songIndex);
		if(!tunes.isEmpty()) {
			tracksModel_->addTunes(tunes);
		}

		int albumsIndex = replyStr.indexOf("<a name=\"albums\"></a>");
		QList<QompPluginModelItem*> albums = parseAlbums(replyStr, albumsIndex);
		if(!albums.isEmpty()) {
			albumsModel_->addTopLevelItems(albums);
		}

		int artistsIndex = replyStr.indexOf("<a name=\"artists\"></a>");
		albumsIndex = replyStr.indexOf("<a name=\"albums\"></a>");
		if(artistsIndex != -1) {
			QRegExp artistRx("<td>\\s+<a(\\s+Class=\"darkorange\")?\\s+href=\"([^\"]+)\">([^<]+)</a>\\s+</td>\\s+<td>\\s+(\\d+)\\s+</td>",
					 Qt::CaseInsensitive);
			artistRx.setMinimal(true);
			QList<QompPluginModelItem*> artists;
			while((artistsIndex = artistRx.indexIn(replyStr, artistsIndex)) != -1
					&& artistsIndex < albumsIndex)
			{
				artistsIndex += artistRx.matchedLength();
				QompPluginArtist* artist = new QompPluginArtist();
				artist->artist = unescape(artistRx.cap(3).trimmed());
				artist->internalId = artistRx.cap(2).trimmed();
				bool ok;
				int count = artistRx.cap(4).toInt(&ok);
				QList<QompPluginModelItem*> list;
				if(ok) {
					while(count) {
						list.append(new QompPluginAlbum(artist));
						--count;
					}
				}
				artist->setItems(list);
				artists.append(artist);
			}
			artistsModel_->addTopLevelItems(artists);
		}
	}
}

void MyzukaruGettunesDlg::itemSelected(QompPluginModelItem *item)
{
	if(!item || item->internalId.isEmpty())
		return;

	QUrl url("http://myzuka.ru/");
	const char* slot;
	switch(item->type()) {
	case Qomp::TypeTune:
	{
		QompPluginTune *tune = static_cast<QompPluginTune *>(item);
		if(!tune->url.isEmpty())
			return;
		url.setPath(QString("/Song/GetFileUrl/%1").arg(tune->internalId));
		slot = SLOT(tuneUrlFinished());
		break;
	}
	case Qomp::TypeAlbum:
	{
		QompPluginAlbum *album = static_cast<QompPluginAlbum *>(item);
		if(album->tunesReceived)
			return;
		url.setPath(QUrl::fromPercentEncoding(album->internalId.toLatin1()));
		slot = SLOT(albumUrlFinished());
		break;
	}
	case Qomp::TypeArtist:
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
	QNetworkReply *reply = nam_->get(nr);
	reply->setProperty("id", item->internalId);
	QAbstractItemView* view = qobject_cast<QAbstractItemView*>(sender());
	if(view) {
		requests_.insert(reply, (void*)view->model());
	}

#pragma GCC diagnostic ignored "-Wuninitialized"
	connect(reply, SIGNAL(finished()), slot);
}

void MyzukaruGettunesDlg::tuneUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	void* model = requests_.value(reply);
	requests_.remove(reply);
	if(reply->error() == QNetworkReply::NoError) {
		QString id = reply->property("id").toString();
		QRegExp re("\"(http://[^\"]+)\"");
		QString text = reply->readAll();
		if(re.indexIn(text) != -1) {
			if(model == tracksModel_) {
				QompPluginModelItem* it = tracksModel_->tuneForId(id);
				if(it && it->type() == Qomp::TypeTune) {
					static_cast<QompPluginTune*>(it)->url = re.cap(1);
					tracksModel_->emitUpdateSignal();
				}
				return;
			}
			else if(model == albumsModel_) {
				QompPluginModelItem* it = albumsModel_->itemForId(id);
				if(it && it->type() == Qomp::TypeTune) {
					static_cast<QompPluginTune*>(it)->url = re.cap(1);
					albumsModel_->emitUpdateSignal();
				}
				return;
			}
			else if(model == artistsModel_) {
				QompPluginModelItem* it = artistsModel_->itemForId(id);
				if(it && it->type() == Qomp::TypeTune) {
					static_cast<QompPluginTune*>(it)->url = re.cap(1);
					artistsModel_->emitUpdateSignal();
				}
			}
		}
	}

}

void MyzukaruGettunesDlg::albumUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	void* model = requests_.value(reply);
	requests_.remove(reply);
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginModelItem*> tunes = parseTunes(replyStr, 0);
		if(!tunes.isEmpty()) {
			QString id = reply->property("id").toString();

			if(model == albumsModel_) {
				QompPluginModelItem* it = albumsModel_->itemForId(id);
				if(it && it->type() == Qomp::TypeAlbum) {
					QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
					albumsModel_->setItems(tunes, it);
					foreach(QompPluginModelItem* t, tunes) {
						static_cast<QompPluginTune*>(t)->album = pa->album;
					}
					pa->tunesReceived = true;
				}
				return;
			}
			else if(model == artistsModel_) {
				QompPluginModelItem* it = artistsModel_->itemForId(id);
				if(it && it->type() == Qomp::TypeAlbum) {
					QompPluginAlbum* pa = static_cast<QompPluginAlbum*>(it);
					artistsModel_->setItems(tunes, it);
					foreach(QompPluginModelItem* t, tunes) {
						static_cast<QompPluginTune*>(t)->album = pa->album;
						itemSelected(t);
					}
					pa->tunesReceived = true;
				}
			}
		}
	}
}

void MyzukaruGettunesDlg::artistUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QString id = reply->property("id").toString();
		QompPluginModelItem* it = artistsModel_->itemForId(id);
		artistsModel_->setItems(QList<QompPluginModelItem*>(), it);

		QList<QompPluginModelItem*> albums = parseAlbums(replyStr, 0);
		if(!albums.isEmpty()) {
			if(it && it->type() == Qomp::TypeArtist) {
				QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
				foreach(QompPluginModelItem* t, albums) {
					static_cast<QompPluginAlbum*>(t)->artist = pa->artist;
				}
				pa->tunesReceived = true;
			}
			artistsModel_->addItems(albums, it);
		}

		QList<QompPluginModelItem*> tunes = parseTunes(replyStr, 0);
		if(!tunes.isEmpty()) {
			if(it && it->type() == Qomp::TypeArtist) {
				QompPluginArtist* pa = static_cast<QompPluginArtist*>(it);
				foreach(QompPluginModelItem* t, tunes) {
					static_cast<QompPluginTune*>(t)->artist = pa->artist;
				}
				pa->tunesReceived = true;
			}
			artistsModel_->addItems(tunes, it);
		}
	}
}
