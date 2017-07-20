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
#include "cuteradiomodel.h"
#include "playlistparser.h"
#include "tune.h"

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
static const QString HTML_TYPE = "text/html";
static const QString supportedMimeTypesPrefix = "audio/";

#define TUNE_PROPERTY "tune"
#define STEP_PROPERTY "step"
#define CONTAINER_PROPERTY "container"

DataPairs CuteRadioController::_countries{};
DataPairs CuteRadioController::_genres{};



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

	connect(this, &CuteRadioController::countriesChanged, dlg_, &CuteRadioPluginGetTunesDialog::setCountries);
	connect(this, &CuteRadioController::genresChanged, dlg_, &CuteRadioPluginGetTunesDialog::setGenres);
	if(_countries.isEmpty())
		loadCountries(0);
	else
		emit countriesChanged(&_countries);

	if(_genres.isEmpty())
		loadGenres(0);
	else
		emit genresChanged(&_genres);
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
	model_->reset();
	const QString str = prepareSearchString(text);
	if(!str.isEmpty())
		doSearchStepTwo(str);
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

QString CuteRadioController::prepareSearchString(const QString& str) const
{
	QStringList res;
	QString genre = dlg_->genre();
	QString country = dlg_->country();

	if(!str.isEmpty())
		res.append(QString("search=%1").arg(str));

	if(!genre.isEmpty())
		res.append(QString("genre=%1").arg(genre));

	if(!country.isEmpty())
		res.append(QString("country=%1").arg(country));

	if(res.count() == 0)
		return QString();

	res << QStringLiteral("limit=50") << QStringLiteral("sortDescending=true") << QStringLiteral("sort=lastPlayed");

	return res.join("&");
}

void CuteRadioController::processTunes(QList<Tune *> tunes, QompPluginModelItem *item)
{
	if(tunes.count() > 0) {
		Tune* t = tunes.at(0);

		QNetworkRequest nr(t->url);
		nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
		QNetworkReply *r = nam()->get(nr);
		r->setProperty(TUNE_PROPERTY, QVariant::fromValue<qintptr>(reinterpret_cast<qintptr>(item)));
		r->setProperty(STEP_PROPERTY, 2);

		connect(r, &QNetworkReply::readyRead, this, &CuteRadioController::getUrlReadyRead);
		connect(r, &QNetworkReply::finished, this, &CuteRadioController::getUrlFinished);
		connect(this, &CuteRadioController::destroyed, r, &QNetworkReply::deleteLater);

		startBusy();
	}

	qDeleteAll(tunes);
}

void CuteRadioController::loadGenres(int offset)
{
	QString url = QString("/genres?limit=50&offset=%1&sortDescending=true&sort=count")
			.arg(offset);

	loadFilterData(url, &_genres);
}

void CuteRadioController::loadCountries(int offset)
{
	QString url = QString("/countries?limit=50&offset=%1&sortDescending=true&sort=count")
			.arg(offset);

	loadFilterData(url, &_countries);
}

void CuteRadioController::loadFilterData(const QString &urlPath, QList<QPair<QString, int> > *container)
{
	QNetworkRequest nr(CuteRadioUrl + urlPath);
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	QNetworkReply *reply = nam()->get(nr);
	reply->setProperty(CONTAINER_PROPERTY, reinterpret_cast<qintptr>(container));
	connect(reply, &QNetworkReply::finished, this, &CuteRadioController::getFilterDataFinished);
	connect(this, &CuteRadioController::destroyed, reply, &QNetworkReply::deleteLater);
	startBusy();
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
	if(r->property(STEP_PROPERTY).toInt() == 1) {
		for(const QString& type: PlaylistParser::cupportedMimeTypes()) {
			if(content.startsWith(type, Qt::CaseInsensitive))
				return;
		}
	}

	if(content.contains(supportedMimeTypesPrefix, Qt::CaseInsensitive)) {
		ct->url = r->url().toString();
		model_->emitUpdateSignal(model_->index(ct));
	}
	else if(content.compare(HTML_TYPE, Qt::CaseInsensitive) == 0) { //SHOUTcast server probably
		ct->url = r->url().toString();
		if(!ct->url.endsWith("/"))
			ct->url += "/";
		ct->url += ";";
		model_->emitUpdateSignal(model_->index(ct));
	}

	r->abort();

}

void CuteRadioController::getUrlFinished()
{
	QNetworkReply* r = static_cast<QNetworkReply*>(sender());
	CuteRadioTune* ct = reinterpret_cast<CuteRadioTune*>(r->property(TUNE_PROPERTY).value<qintptr>());
	stopBusy();
	r->deleteLater();

	if(r->error() == QNetworkReply::NoError) {
		const QString content = r->header(QNetworkRequest::ContentTypeHeader).toString();
		QByteArray data = r->readAll();
#ifdef DEBUG_OUTPUT
		qDebug() << "CuteRadioController::getUrlFinished() " << content << data;
#endif
		PlaylistParser p(data, content);
		if(p.canParse()) {
			processTunes(p.parse(), ct);
		}
	}
}

void CuteRadioController::getFilterDataFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	stopBusy();
	if(reply->error() == QNetworkReply::NoError) {
		DataPairs* container = reinterpret_cast<DataPairs*>(reply->property(CONTAINER_PROPERTY).value<qintptr>());
		QByteArray ba = reply->readAll();
		QJsonDocument doc = QJsonDocument::fromJson(ba);
		QJsonObject jo = doc.object();

		if(jo.contains("items")) {
			QJsonArray arr = jo.value("items").toArray();
			for(const QJsonValue& item: arr) {
				QJsonObject obj = item.toObject();
				container->append(qMakePair(obj.value("name").toString(), obj.value("count").toInt()));
			}
		}

		if(jo.contains("next")) {
			const QString next = jo.value("next").toString();
			loadFilterData(next, container);
		}

		if(container == &_countries)
			emit countriesChanged(container);
		else
			emit genresChanged(container);
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
	r->setProperty(STEP_PROPERTY, 1);

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
