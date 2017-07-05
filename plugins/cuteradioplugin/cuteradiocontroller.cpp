/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#include "cuteradiocontroller.h"
#include "common.h"
#include "options.h"
#include "cuteradioplugindefines.h"
#include "tune.h"
#include "cuteradioplugingettunesdialog.h"
#include "cuteradiomodel.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>


static const QString CuteRadioUrl = "http://marxoft.co.uk/api/cuteradio";



CuteRadioController::CuteRadioController(QObject *parent) :
	QompPluginController(parent),
	model_(new CuteRadioModel(this)),
	dlg_(new CuteRadioPluginGetTunesDialog()),
	searchesCount_(0)
{
	init();	
}

void CuteRadioController::init()
{
	QompPluginController::init();

	dlg_->setModel(model_);
}

CuteRadioController::~CuteRadioController()
{
	delete dlg_;
}

QList<Tune*> CuteRadioController::prepareTunes() const
{
	QList<Tune*> tunes;
	foreach(QompPluginModelItem* item, model_->selectedItems()) {
		if(!item)
			continue;

		QompPluginTune *pt = static_cast<QompPluginTune*>(item);
		if(!pt->url.isNull()) {
			tunes.append(pt->toTune());
		}
	}
	return tunes;
}

void CuteRadioController::doSearch(const QString& text)
{
	if(text.isEmpty())
		return;

	model_->reset();
	doSearchStepTwo(parseSearchString(text));
}

QompPluginGettunesDlg *CuteRadioController::view() const
{
	return dlg_;
}

void CuteRadioController::doSearchStepTwo(const QString &str)
{
	QString url = QString("%1/stations?%2").arg(CuteRadioUrl, str);

	QNetworkRequest nr(url);
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	QNetworkReply *reply = nam()->get(nr);
	connect(reply, &QNetworkReply::finished, this, &CuteRadioController::searchFinished);
	startBusy();
}

void CuteRadioController::startBusy()
{
	++searchesCount_;
	dlg_->startBusyWidget();
}

void CuteRadioController::stopBusy()
{
	if(--searchesCount_ == 0)
		dlg_->stopBusyWidget();
}

QString CuteRadioController::parseSearchString(QString str) const
{
	QStringList parts = str.replace(",", " ").split(" ", QString::SkipEmptyParts);
	QStringList res;
	for(int i = 0; i < parts.count(); ++i) {
		const QString& part = parts.at(i);

		if(part.compare(QStringLiteral("genre:")) == 0) {
			res.append( QStringLiteral("genre=") + parts.at(++i) );
		}
		else if(part.compare(QStringLiteral("language:")) == 0) {
			res.append( QStringLiteral("language=") + parts.at(++i) );
		}
		else if(part.compare(QStringLiteral("country:")) == 0) {
			res.append( QStringLiteral("country=") + parts.at(++i) );
		}
		else {
			res.append( QStringLiteral("search=") + part );
		}
	}
	return res.join("&") + QStringLiteral("&limit=50");
}

void CuteRadioController::searchFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	stopBusy();
	if(reply->error() == QNetworkReply::NoError) {
		QByteArray ba = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(ba);
		QJsonObject jo = doc.object();

		if(jo.contains("items")) {
			QJsonArray arr = jo.value("items").toArray();
			for(const QJsonValue& item: arr) {
				QJsonObject obj = item.toObject();
				if(!obj.contains("lastPlayed")) {
					continue;
				}
				auto dt = obj.value("lastPlayed").toVariant().toDateTime();
				if(dt.isValid() && dt.daysTo(QDateTime::currentDateTime()) < 100) {
					CuteRadioTune *t = new CuteRadioTune;

					if(obj.contains("title"))
						t->title = obj.value("title").toString();

					if(obj.contains("description"))
						t->artist = obj.value("description").toString();

					if(obj.contains("source"))
						t->url = obj.value("source").toString();

					t->lastPlayed = dt.toString(Qt::SystemLocaleShortDate);

					model_->addTopLevelItems({t});
				}
			}
		}

		if(jo.contains("next")) {
			QString next = jo.value("next").toString();
			doSearchStepTwo(next);
		}
	}
	else {
		dlg_->showAlert(tr("Error"), reply->errorString());
	}
}

void CuteRadioController::itemSelected(QompPluginModelItem* item)
{
	Q_UNUSED(item)
}

void CuteRadioController::getSuggestions(const QString &text)
{
	Q_UNUSED(text)
}

void CuteRadioController::suggestionsFinished()
{
}
