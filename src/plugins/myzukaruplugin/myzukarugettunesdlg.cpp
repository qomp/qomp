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

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QDebug>

MyzukaruGettunesDlg::MyzukaruGettunesDlg(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	tabWidget_(new QTabWidget(this)),
	tracksView_(new QompPluginTracksView(this))
{
	QompPluginTracksModel* tm = new MyzukaruTracksModel(this);
	tracksView_->setModel(tm);
	tabWidget_->addTab(tracksView_, tr("Tracks"));
	setResultsWidget(tabWidget_);

	connect(tracksView_, SIGNAL(tuneSelected(QompPluginTune)), SLOT(tuneSelected(QompPluginTune)));
}

void MyzukaruGettunesDlg::accept()
{
	QompPluginTracksModel* tm = qobject_cast<QompPluginTracksModel*>(tracksView_->model());
	if (tm)
		for(int i = 0; i < tm->rowCount(); i++) {
			QModelIndex index = tm->index(i);
			if(index.data(Qt::CheckStateRole).toBool()){
				QompPluginTune mt = tm->tune(index);
				if(mt.url.isNull())
					continue;

				Tune tune;
				tune.artist = mt.artist;
				tune.title = mt.title;
				tune.url = mt.url;
				//tune.duration = pt.durationToStr();
				tunes_.append(tune);
			}
		}

	QDialog::accept();
}

void MyzukaruGettunesDlg::doSearch()
{
	const QString txt = currentSearchText();
	if(txt.isEmpty())
		return;

	const QString url = QString("http://myzuka.ru/Search?searchText=%1").arg(txt);
	QNetworkRequest nr;
	nr.setUrl(url);
	QNetworkReply* reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
}

void MyzukaruGettunesDlg::searchFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();

	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		int songIndex = replyStr.indexOf("<a name=\"songs\"></a>");
		if(songIndex != -1) {
			QRegExp songRx("<a Class=\"darkorange\"\\s+href=\"([^\"]+)\">([^<]+)</a>\\s+"
				       "</td>\\s+<td>\\s+"
				       "<a Class=\"darkorange\"\\s+href=\"([^\"]+)\">([^<]+)</a>\\s+</td>");
			songRx.setMinimal(true);
			QList<QompPluginTune> tunes;
			while((songIndex = songRx.indexIn(replyStr, songIndex)) != -1) {
				songIndex += songRx.matchedLength();
				QompPluginTune t;
				t.artist = songRx.cap(2);
				t.title = songRx.cap(4);
//				t.url = songRx.cap(3);
				QRegExp idRx("/(\\d+)/");
				if(idRx.indexIn(songRx.cap(3)) != -1)
					t.id = idRx.cap(1);
				tunes.append(t);
			}
			QompPluginTracksModel* tm = qobject_cast<QompPluginTracksModel*>(tracksView_->model());
			if (tm)
				tm->addTunes(tunes);
		}

	}
}

void MyzukaruGettunesDlg::urlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString id = reply->property("id").toString();
		QRegExp re("\"(http://[^\"]+)\"");
		QString text = reply->readAll();
		if(re.indexIn(text) != -1) {
			QompPluginTracksModel* tm = qobject_cast<QompPluginTracksModel*>(tracksView_->model());
			if (tm)
				tm->urlChanged(id, re.cap(1));
		}
	}
}

void MyzukaruGettunesDlg::tuneSelected(const QompPluginTune &tune)
{
	QUrl url("http://myzuka.ru/");
	url.setPath(QString("/Song/GetFileUrl/%1").arg(tune.id));
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = nam_->get(nr);
	reply->setProperty("id", tune.id);
	connect(reply, SIGNAL(finished()), SLOT(urlFinished()));
}
