/*
 * Copyright (C) 2018  Khryukin Evgeny
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

#include "poiskmcontroller.h"
#include "poiskmplugingettunesdialog.h"
#include "qompplugintypes.h"
#include "common.h"
#include "qompplugintreemodel.h"
#include "options.h"
#include "poiskmplugindefines.h"
#include "tune.h"
#include "poiskmurlresolvestrategy.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>
#include <QStringList>


static const QString SongString("<div .+ data-songid=\"([^\"]+)\"\\s+class=\"title_wrap\">"
		".+<a data-searchitq=[^>]+>([^<]+)</a>.+<span\\s+class=\"title-name\">([^<]+)</span>"
		".+<span\\s+class=\"duration\">([^<]+)</span>");

class PoiskmrTune : public QompPluginTune
{
public:
	PoiskmrTune() : QompPluginTune() {}

//	QString durationToStr() const
//	{
//		return Qomp::durationSecondsToString(duration.toInt());
//	}

	virtual Tune* toTune() const
	{
		Tune* t = QompPluginTune::toTune();
		t->setUrlResolveStrategy(PoiskmURLResolveStrategy::instance());
		return t;
	}
};

PoiskmController::PoiskmController(QObject *parent) :
	QompPluginController(parent),
	model_(new QompPluginTreeModel(this)),
	dlg_(new PoiskmPluginGetTunesDialog()),
	searchesCount_(0)
{
	init();	
}

void PoiskmController::init()
{
	QompPluginController::init();

	dlg_->setModel(model_);

	connect(dlg_, &PoiskmPluginGetTunesDialog::next, this, &PoiskmController::actNextActivated);
	connect(dlg_, &PoiskmPluginGetTunesDialog::prev, this, &PoiskmController::actPrevActivated);
}

PoiskmController::~PoiskmController()
{
	delete dlg_;
}

QList<Tune*> PoiskmController::prepareTunes() const
{
	QList<Tune*> tunes;
	foreach(QompPluginModelItem* item, model_->selectedItems()) {
		if(!item)
			continue;

		PoiskmrTune *pt = static_cast<PoiskmrTune*>(item);
		if(!pt->url.isNull()) {
			tunes.append(pt->toTune());
		}
	}
	return tunes;
}

void PoiskmController::doSearch(const QString& text)
{
	if(text.isEmpty())
		return;

	model_->reset();
	dlg_->setPage(0);
	lastSearchStr_ = text;
	doSearchStepTwo();
}

QompPluginGettunesDlg *PoiskmController::view() const
{
	return dlg_;
}

void PoiskmController::doSearchStepTwo()
{
	QString url = QString("https://%1/show/%2").arg(POISKM_PLUGIN_URL, lastSearchStr_);
	int page = dlg_->page();

	if(page > 0) {
		url += QString("?page=%1").arg(QString::number(++page));
	}
	QNetworkRequest nr(url);
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	nr.setRawHeader("Referer", QString("https://%1").arg(POISKM_PLUGIN_URL).toLatin1());
	nr.setRawHeader("Host", POISKM_PLUGIN_URL);
	QNetworkReply *reply = nam()->get(nr);
	connect(reply, &QNetworkReply::finished, this, &PoiskmController::searchFinished);
	connect(this, &PoiskmController::destroyed, reply, &QNetworkReply::deleteLater);
	startBusy();
}

void PoiskmController::startBusy()
{
	++searchesCount_;
	dlg_->startBusyWidget();
}

void PoiskmController::stopBusy()
{
	if(--searchesCount_ == 0)
		dlg_->stopBusyWidget();
}

void PoiskmController::searchFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	stopBusy();
	if(reply->error() == QNetworkReply::NoError || reply->error() == QNetworkReply::ContentNotFoundError) {
		QRegExp re(SongString);
		re.setMinimal(true);
		QList<QompPluginModelItem*> list;
		const QString result = QString::fromUtf8(reply->readAll());
		int off = 0;
		while((off = re.indexIn(result, off)) != -1) {
			off += re.matchedLength();
			PoiskmrTune* tune = new PoiskmrTune();
			tune->artist = Qomp::unescape(re.cap(2));
			tune->title = Qomp::unescape(re.cap(3));
			tune->internalId = re.cap(1);
			tune->duration = re.cap(4);
			list.append(tune);
		}

		dlg_->enableNext(false);
		if(!list.isEmpty()) {
			int page = dlg_->page();
			QRegExp curPage("<li\\s+class=\"active\">\\s*<a href=\"[^\"]+\">([\\d]+)</a>");
			if(curPage.indexIn(result) != -1) {
				int newPage = curPage.cap(1).toInt();
				if(newPage > page) {
					dlg_->setPage(newPage);
					model_->addTopLevelItems(list);
					dlg_->enableNext(true);
				}
			}


		}
	}
	else {
		dlg_->showAlert(tr("Error"), reply->errorString());
	}
}

void PoiskmController::actPrevActivated()
{
}

void PoiskmController::actNextActivated()
{
	doSearchStepTwo();
}

void PoiskmController::itemSelected(QompPluginModelItem* item)
{
	PoiskmrTune* pt = static_cast<PoiskmrTune*>(item);
	if(!pt->url.isEmpty())
		return;

	pt->url = pt->internalId;
	QModelIndex ind = model_->index(pt);
	model_->emitUpdateSignal(ind);
}

void PoiskmController::getSuggestions(const QString &/*text*/)
{
}

void PoiskmController::suggestionsFinished()
{
}
