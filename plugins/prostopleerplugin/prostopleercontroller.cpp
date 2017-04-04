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

#include "prostopleercontroller.h"
#include "prostopleerplugingettunesdialog.h"
#include "qompplugintypes.h"
#include "common.h"
#include "qompplugintreemodel.h"
#include "options.h"
#include "prostopleerplugindefines.h"
#include "tune.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>


static const QString ProstoPleerUrl = "http://pleer.net";

class ProstopleerTune : public QompPluginTune
{
public:
	ProstopleerTune() : QompPluginTune() {}

	QString durationToStr() const
	{
		return Qomp::durationSecondsToString(duration.toInt());
	}

	virtual Tune* toTune() const
	{
		Tune* t = QompPluginTune::toTune();
		t->duration = durationToStr();
		return t;
	}
};

ProstoPleerController::ProstoPleerController(QObject *parent) :
	QompPluginController(parent),
	model_(new QompPluginTreeModel(this)),
	dlg_(new ProstoPleerPluginGetTunesDialog()),
	searchesCount_(0)
{
	init();	
}

void ProstoPleerController::init()
{
	QompPluginController::init();

	dlg_->setModel(model_);

	connect(dlg_, SIGNAL(next()), SLOT(actNextActivated()));
	connect(dlg_, SIGNAL(prev()), SLOT(actPrevActivated()));

	doLogin();
}

ProstoPleerController::~ProstoPleerController()
{
	delete dlg_;
}

QList<Tune*> ProstoPleerController::prepareTunes() const
{
	QList<Tune*> tunes;
	foreach(QompPluginModelItem* item, model_->selectedItems()) {
		if(!item)
			continue;

		ProstopleerTune *pt = static_cast<ProstopleerTune*>(item);
		if(!pt->url.isNull()) {
			tunes.append(pt->toTune());
		}
	}
	return tunes;
}

void ProstoPleerController::doSearch(const QString& text)
{
	if(text.isEmpty())
		return;

	model_->reset();
	dlg_->setPage(0);
	lastSearchStr_ = text;
	doSearchStepTwo();
}

QompPluginGettunesDlg *ProstoPleerController::view() const
{
	return dlg_;
}

void ProstoPleerController::doSearchStepTwo()
{
	QString url = QString("%1/search?q=%2").arg(ProstoPleerUrl, lastSearchStr_);
	int page = dlg_->page();

	if(page > 0) {
		url += QString("&page=%1").arg(QString::number(++page));
	}
	QNetworkRequest nr(url);
	QNetworkReply *reply = nam()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
	startBusy();
}

void ProstoPleerController::startBusy()
{
	++searchesCount_;
	dlg_->startBusyWidget();
}

void ProstoPleerController::stopBusy()
{
	if(--searchesCount_ == 0)
		dlg_->stopBusyWidget();
}

void ProstoPleerController::searchFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	stopBusy();
	if(reply->error() == QNetworkReply::NoError) {
		QRegExp re("<li duration=\"([\\d]+)\"\\s+file_id=\"([^\"]+)\"\\s+singer=\"([^\"]+)\"\\s+"
			   "song=\"([^\"]+)\"\\s+link=\"([^\"]+)\"");
		re.setMinimal(true);
		QList<QompPluginModelItem*> list;
		QString result = QString::fromUtf8(reply->readAll());
		int off = 0;
		while((off = re.indexIn(result, off)) != -1) {
			off += re.matchedLength();
			ProstopleerTune* tune = new ProstopleerTune();
			tune->artist = Qomp::unescape(re.cap(3));
			tune->title = Qomp::unescape(re.cap(4));
			tune->internalId = re.cap(5);
			tune->duration = re.cap(1);
			list.append(tune);
		}
		if(!list.isEmpty())
			model_->addTopLevelItems(list);

//		QRegExp page("<ul class=\"pagination\" end=\"(\\d+)\"");
//		if(page.indexIn(result) != -1) {
//			dlg_->setTotalPages(page.cap(1).toInt());
//		}
		QRegExp curPage("&page=(\\d+)");
		if(curPage.indexIn(reply->url().toString()) != -1) {
			dlg_->setPage(curPage.cap(1).toInt());
		}
		else {
			dlg_->setPage(1);
		}

//		int cur = dlg_->page();
//		int total = dlg_->totalPages();
		dlg_->enableNext(!list.isEmpty());
	}
	else {
		dlg_->showAlert(tr("Error"), reply->errorString());
	}
}

void ProstoPleerController::actPrevActivated()
{
}

void ProstoPleerController::actNextActivated()
{
	doSearchStepTwo();
}

void ProstoPleerController::itemSelected(QompPluginModelItem* item)
{
	ProstopleerTune* pt = static_cast<ProstopleerTune*>(item);
	if(!pt->url.isEmpty())
		return;

	QUrl url(ProstoPleerUrl);
	url.setPath("/site_api/files/get_url");
	QNetworkRequest nr(url);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	nr.setRawHeader("Accept", "application/json, text/javascript");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QByteArray ba("action=play&id=");
	ba += pt->internalId;
	QNetworkReply *reply = nam()->post(nr, ba);
	reply->setProperty("id", pt->internalId);
	connect(reply, SIGNAL(finished()), SLOT(urlFinished()));
	startBusy();
}

void ProstoPleerController::urlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	stopBusy();
	if(reply->error() == QNetworkReply::NoError) {
		QString id = reply->property("id").toString();
		QRegExp re("http://[^\"]+");
		QString text = reply->readAll();
		if(re.indexIn(text) != -1) {
			ProstopleerTune* pt = static_cast<ProstopleerTune*>(model_->itemForId(id));
			pt->url = re.cap();
			QModelIndex ind = model_->index(pt);
			model_->emitUpdateSignal(ind);
		}
	}
}

void ProstoPleerController::doLogin()
{
	QUrl url(ProstoPleerUrl);
	url.setPath("/login");
	QNetworkRequest nr(url);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	nr.setRawHeader("Accept", "application/json, text/javascript");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QString str = QString("login=%1&password=%2").arg(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN).toString(),
				Qomp::decodePassword(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD).toString(), PROSTOPLEER_DECODE_KEY));
	QNetworkReply *reply = nam()->post(nr, str.toLatin1());
	connect(reply, SIGNAL(finished()), SLOT(loginFinished()));
}

void ProstoPleerController::loginFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	QRegExp re("\"success\":([^,]+)");
	QString result = reply->readAll();
	if(re.indexIn(result) != -1) {
		if(re.cap(1) == "true") {
			dlg_->setAuthStatus(tr("OK"));
			return;
		}
	}
	dlg_->setAuthStatus(tr("None"));
}

void ProstoPleerController::getSuggestions(const QString &text)
{
	QUrl url(ProstoPleerUrl);
	url.setPath("/search_suggest");
	QNetworkRequest nr(url);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	nr.setRawHeader("Accept", "application/json, text/javascript");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QByteArray ba("part=");
	ba += text.toLatin1();
	QNetworkReply *reply = nam()->post(nr, ba);
	connect(reply, SIGNAL(finished()), SLOT(suggestionsFinished()));
}

void ProstoPleerController::suggestionsFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString text = QString::fromUtf8(reply->readAll());
		QRegExp re("\"([^\"]+)\"");
		re.setMinimal(true);
		int index = text.indexOf("[");
		QStringList sugs;
		while( (index = re.indexIn(text, index)) != -1 ) {
			index += re.matchedLength();
			sugs.append(Qomp::unescape(re.cap(1)));
		}
		if(!sugs.isEmpty())
			emit suggestionsReady(sugs);
	}
}
