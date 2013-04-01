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
#include "qomppluginalbumsview.h"
#include "common.h"
#include "myzukarudefines.h"
#include "qompplugintypes.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QtDebug>
MyzukaruGettunesDlg::MyzukaruGettunesDlg(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	albumsView_(new QompPluginAlbumsView(this)),
	artistsView_(new QompPluginAlbumsView(this)),
	tracksView_(new QompPluginTracksView(this))
{
	setWindowTitle(MYZUKA_PLUGIN_NAME);

	MyzukaruTracksModel* tm = new MyzukaruTracksModel(this);
	tracksView_->setModel(tm);

	MyzukaruAlbumsModel* am = new MyzukaruAlbumsModel(this);
	albumsView_->setModel(am);

	MyzukaruArtistsModel* arm = new MyzukaruArtistsModel(this);
	artistsView_->setModel(arm);

	QTabWidget* tabWidget = new QTabWidget(this);
	tabWidget->addTab(albumsView_, tr("Albums"));
	tabWidget->addTab(tracksView_, tr("Tracks"));
	tabWidget->addTab(artistsView_, tr("Artists"));
	setResultsWidget(tabWidget);

	connect(tracksView_, SIGNAL(tuneSelected(QompPluginTune*)), SLOT(tuneSelected(QompPluginTune*)));
	connect(albumsView_, SIGNAL(albumSelected(QompPluginAlbum*)), SLOT(albumSelected(QompPluginAlbum*)));
	connect(albumsView_, SIGNAL(tuneSelected(QompPluginTune*)), SLOT(tuneSelected(QompPluginTune*)));
	connect(artistsView_, SIGNAL(tuneSelected(QompPluginTune*)), SLOT(tuneSelected(QompPluginTune*)));
	connect(artistsView_, SIGNAL(albumSelected(QompPluginAlbum*)), SLOT(albumSelected(QompPluginAlbum*)));
}

void MyzukaruGettunesDlg::accept()
{
	MyzukaruTracksModel* tm = qobject_cast<MyzukaruTracksModel*>(tracksView_->model());
	if (tm) {
		foreach(QompPluginTune* t,  tm->selectedTunes()) {
			addTune(t);
		}
	}

	MyzukaruAlbumsModel* am = qobject_cast<MyzukaruAlbumsModel*>(albumsView_->model());
	if (am) {
		foreach(QompPluginTune* t, am->selectedTunes()) {
			addTune(t);
		}
	}

	QDialog::accept();
}

void MyzukaruGettunesDlg::addTune(QompPluginTune *tune)
{
	if(!tune || tune->url.isEmpty())
		return;

	Tune t;
	t.artist = tune->artist;
	t.title = tune->title;
	t.album == tune->album;
	t.url = tune->url;
	tunes_.append(t);
}

void MyzukaruGettunesDlg::doSearch()
{
	const QString txt = currentSearchText();
	if(txt.isEmpty())
		return;

	MyzukaruTracksModel* tm = qobject_cast<MyzukaruTracksModel*>(tracksView_->model());
	if (tm)
		tm->reset();
	MyzukaruAlbumsModel* am = qobject_cast<MyzukaruAlbumsModel*>(albumsView_->model());
	if (am)
		am->reset();
	MyzukaruArtistsModel* arm = qobject_cast<MyzukaruArtistsModel*>(artistsView_->model());
	if (arm)
		arm->reset();

	const QString url = QString("http://myzuka.ru/Search?searchText=%1").arg(txt);
	QNetworkRequest nr;
	nr.setUrl(url);
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
}

static QList<QompPluginTune*> parseTunes(const QString& replyStr, int songIndex)
{
	QList<QompPluginTune*> tunes;
	if(songIndex != -1) {
		QRegExp songRx("<a(\\s+Class=\"darkorange\")?\\s+(class=\"link_orange\")?href=\"([^\"]+)\">([^<]+)</a>\\s+"
			       "</td>\\s+<td>\\s+"
			       "<a(\\s+Class=\"darkorange\")?\\s+(class=\"link_orange\")?href=\"([^\"]+)\">([^<]+)</a>", Qt::CaseInsensitive);
		songRx.setMinimal(true);
		while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
			songIndex += songRx.matchedLength();
			QRegExp idRx("/(\\d+)/");
			if(idRx.indexIn(songRx.cap(7)) != -1) {
				QompPluginTune* t = new QompPluginTune();
				t->id = idRx.cap(1);
				t->artist = unescape(songRx.cap(4));
				t->title = unescape(songRx.cap(8));

				tunes.append(t);
			}
		}
	}
	return tunes;
}

void MyzukaruGettunesDlg::searchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		int songIndex = replyStr.indexOf("<a name=\"songs\"></a>");
		QList<QompPluginTune*> tunes = parseTunes(replyStr, songIndex);
		if(!tunes.isEmpty()) {
			MyzukaruTracksModel* tm = qobject_cast<MyzukaruTracksModel*>(tracksView_->model());
			if (tm)
				tm->addTunes(tunes);
		}
		int albumsIndex = replyStr.indexOf("<a name=\"albums\"></a>");
		if(albumsIndex != -1) {
			QRegExp albumRx("<a Class=\"darkorange\" href=\"([^\"]+)\">([^<]+)</a>\\s+</td>\\s+<td>\\s+"
					"<a class=\"darkorange\" href=\"([^\"]+)\">([^<]+)</a>\\s+</td>\\s+<td>\\s+(\\d+)\\s+"
					"</td>\\s+<td>\\s+(\\d+)\\s+</td>", Qt::CaseInsensitive);
			albumRx.setMinimal(true);
			QList<QompPluginAlbum*> albums;
			while((albumsIndex = albumRx.indexIn(replyStr, albumsIndex)) != -1) {
				albumsIndex += albumRx.matchedLength();
				QompPluginAlbum* album = new QompPluginAlbum();
				album->artist = unescape(albumRx.cap(2).trimmed());
				album->album = unescape(albumRx.cap(4).trimmed());
				album->year = albumRx.cap(5);
				album->id = albumRx.cap(3).trimmed();
				bool ok;
				int count = albumRx.cap(6).toInt(&ok);
				if(ok) {
					while(count) {
						album->tunes.append(new QompPluginTune());
						--count;
					}
				}
				albums.append(album);
			}
			MyzukaruAlbumsModel *am = qobject_cast<MyzukaruAlbumsModel*>(albumsView_->model());
			if (am)
				am->addAlbums(albums);
		}
		int artistsIndex = replyStr.indexOf("<a name=\"artists\"></a>");
		albumsIndex = replyStr.indexOf("<a name=\"albums\"></a>");
		if(artistsIndex != -1) {
			QRegExp artistRx("<td>\\s+<a(\\s+Class=\"darkorange\")?\\s+href=\"([^\"]+)\">([^<]+)</a>\\s+</td>\\s+<td>\\s+(\\d+)\\s+</td>",
					Qt::CaseInsensitive);
			artistRx.setMinimal(true);
			QList<QompPluginAlbum*> albums;
			while((artistsIndex = artistRx.indexIn(replyStr, artistsIndex)) != -1
					&& artistsIndex < albumsIndex)
			{
				artistsIndex += artistRx.matchedLength();
				QompPluginAlbum* album = new QompPluginAlbum();
				album->artist = unescape(artistRx.cap(3).trimmed());
				album->id = artistRx.cap(2).trimmed();
				bool ok;
				int count = artistRx.cap(4).toInt(&ok);
				if(ok) {
					while(count) {
						album->tunes.append(new QompPluginTune());
						--count;
					}
				}
				albums.append(album);
			}
			MyzukaruArtistsModel *am = qobject_cast<MyzukaruArtistsModel*>(artistsView_->model());
			if (am)
				am->addAlbums(albums);
		}
	}
}

void MyzukaruGettunesDlg::tuneSelected(QompPluginTune *tune)
{
	if(!tune || tune->id.isEmpty() || !tune->url.isEmpty())
		return;

	QUrl url("http://myzuka.ru/");
	url.setPath(QString("/Song/GetFileUrl/%1").arg(tune->id));
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = nam_->get(nr);
	reply->setProperty("id", tune->id);
	connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinished()));
}

void MyzukaruGettunesDlg::tuneUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString id = reply->property("id").toString();
		QRegExp re("\"(http://[^\"]+)\"");
		QString text = reply->readAll();
		if(re.indexIn(text) != -1) {
			MyzukaruTracksModel* tm = qobject_cast<MyzukaruTracksModel*>(tracksView_->model());
			if (tm && tm->urlChanged(id, re.cap(1)))
				return;
			MyzukaruAlbumsModel* am = qobject_cast<MyzukaruAlbumsModel*>(albumsView_->model());
			if(am && am->urlChanged(id, re.cap(1)))
				return;
		}
	}
}

void MyzukaruGettunesDlg::albumSelected(QompPluginAlbum *album)
{
	if(!album || album->id.isEmpty() || album->tunesReceived)
		return;

	QUrl url("http://myzuka.ru/");
	url.setPath(QUrl::fromPercentEncoding(album->id.toLatin1()));
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = nam_->get(nr);
	reply->setProperty("id", album->id);
	connect(reply, SIGNAL(finished()), SLOT(albumUrlFinished()));
}

void MyzukaruGettunesDlg::albumUrlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QList<QompPluginTune*> tunes = parseTunes(replyStr, 0);
		if(!tunes.isEmpty()) {
			MyzukaruAlbumsModel* tm = qobject_cast<MyzukaruAlbumsModel*>(albumsView_->model());
			if (tm) {
				QString id = reply->property("id").toString();
				tm->album(id)->tunesReceived = true;
				tm->setTunes(tunes, id);
				QompPluginAlbum* a = tm->album(id);
				foreach(QompPluginTune* t, tunes) {
					t->album = a->album;
					tuneSelected(t);
				}
			}
		}
	}
}
