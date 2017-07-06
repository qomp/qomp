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
#include "cuteradioplugindefines.h"
#include "cuteradioplugingettunesdialog.h"
#include "cuteradiomodel.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QTimer>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif


static const QString CuteRadioUrl = "http://marxoft.co.uk/api/cuteradio";
static const QString M3U_TYPE = "audio/x-mpegurl";
static const QString PLS_TYPE = "audio/x-scpls";
static const QString HTML_TYPE = "text/html";
static const QString supportedMimeTypesPrefix = "audio/";

#define TUNE_PROPERTY "tune"



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
	connect(this, &CuteRadioController::destroyed, reply, &QNetworkReply::deleteLater);
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
	return res.join("&") + QStringLiteral("&limit=50&sortDescending=true&sort=lastPlayed");
}

void CuteRadioController::parceM3U(QompPluginModelItem *item, const QString &m3u)
{
	const QStringList lines = m3u.split("\n", QString::SkipEmptyParts);
	for (const QString& line: lines) {
		if (line.isEmpty() || line[0] == '#' || line.size() > 4096)
			continue;

		static_cast<CuteRadioTune*>(item)->url = line;
		model_->emitUpdateSignal(model_->index(item));
		break;
	}
}

void CuteRadioController::parcePLS(QompPluginModelItem *item, const QString &pls)
{
	const QStringList lines = pls.split("\n", QString::SkipEmptyParts);
	for (const QString& line: lines) {
		if (line.startsWith("File") && line.contains("=")) {
			QStringList sl = line.split("=", QString::SkipEmptyParts);
			static_cast<CuteRadioTune*>(item)->url = sl.last();
			model_->emitUpdateSignal(model_->index(item));
			break;
		}
	}
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
				if(dt.isValid() /*&& dt.daysTo(QDateTime::currentDateTime()) < 100*/) {
					CuteRadioTune *t = new CuteRadioTune;

					if(obj.contains("title"))
						t->title = obj.value("title").toString();

					if(obj.contains("description")) {
						const QString descr = obj.value("description").toString();
						if(descr != "-")
							t->artist = descr;
					}

					if(obj.contains("source"))
						t->internalId = obj.value("source").toString();

					if(obj.contains("country"))
						t->country = obj.value("country").toString();

					if(obj.contains("genre"))
						t->genre = obj.value("genre").toString();

					if(obj.contains("language"))
						t->lang = obj.value("language").toString();

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

void CuteRadioController::getUrlReadyRead()
{
	QNetworkReply* r = static_cast<QNetworkReply*>(sender());
	CuteRadioTune* ct = reinterpret_cast<CuteRadioTune*>(r->property(TUNE_PROPERTY).value<qintptr>());
	const QString content = r->header(QNetworkRequest::ContentTypeHeader).toString();
#ifdef DEBUG_OUTPUT
	qDebug() << "getUrlReadyRead()" << content;
#endif
	if( !content.startsWith(M3U_TYPE, Qt::CaseInsensitive)
		&& !content.startsWith(PLS_TYPE, Qt::CaseInsensitive)
	) {
		if(content.contains(supportedMimeTypesPrefix, Qt::CaseInsensitive)) {
			ct->url = r->url().toString();
			model_->emitUpdateSignal(model_->index(ct));
		}
		else if(content.compare(HTML_TYPE, Qt::CaseInsensitive) == 0) { //SHOUTcast server probably
			ct->url = r->url().toString() + ";";
			model_->emitUpdateSignal(model_->index(ct));
		}

		r->abort();
	}
}

void CuteRadioController::getUrlFinished()
{
	QNetworkReply* r = static_cast<QNetworkReply*>(sender());
	CuteRadioTune* ct = reinterpret_cast<CuteRadioTune*>(r->property(TUNE_PROPERTY).value<qintptr>());
	stopBusy();
	r->deleteLater();

	if(r->error() == QNetworkReply::NoError) {
		const QString content = r->header(QNetworkRequest::ContentTypeHeader).toString();
		const QString data = r->readAll();
		if(content.startsWith(M3U_TYPE, Qt::CaseInsensitive)) {
			parceM3U(ct, data);
		}
		else if (content.startsWith(PLS_TYPE, Qt::CaseInsensitive)) {
			parcePLS(ct, data);
		}
	}
}

void CuteRadioController::itemSelected(QompPluginModelItem* item)
{
	CuteRadioTune* ct = static_cast<CuteRadioTune*>(item);
	if(!ct->url.isEmpty())
		return;

	QNetworkRequest nr(ct->internalId);
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	QNetworkReply *r = nam()->get(nr);
	r->setProperty(TUNE_PROPERTY, QVariant::fromValue<qintptr>(reinterpret_cast<qintptr>(item)));

	connect(r, &QNetworkReply::readyRead, this, &CuteRadioController::getUrlReadyRead);
	connect(r, &QNetworkReply::finished, this, &CuteRadioController::getUrlFinished);
	connect(this, &CuteRadioController::destroyed, r, &QNetworkReply::deleteLater);

	startBusy();
}

void CuteRadioController::getSuggestions(const QString &text)
{
	Q_UNUSED(text)
	QTimer::singleShot(100, this, &CuteRadioController::suggestionsFinished);
}

void CuteRadioController::suggestionsFinished()
{
	emit suggestionsReady(QStringList());
}
