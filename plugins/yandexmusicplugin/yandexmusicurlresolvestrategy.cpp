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

#include "yandexmusicurlresolvestrategy.h"
#include "qompnetworkingfactory.h"
#include "yandexmusicoauth.h"

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomComment>
#include <QDomElement>
#include <QtGlobal>

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

static const int TimerInterval = 5000;
//static const QString storageURL = "https://storage.mds.yandex.net";
static const QString apiURL = "https://api.music.yandex.net";

class YandexMusicURLResolveStrategyPrivate : public QObject
{
	Q_OBJECT
public:
	explicit YandexMusicURLResolveStrategyPrivate(const Tune* t) :
		QObject(),
		loop_(new QEventLoop(this)),
		timer_(new QTimer(this)),
		tune_(const_cast<Tune*>(t))
	{
		nam_ = QompNetworkingFactory::instance()->getThreadedNAM();
		timer_->setSingleShot(true);
		timer_->setInterval(TimerInterval);
		connect(timer_, &QTimer::timeout, loop_, &QEventLoop::quit);
	}

	~YandexMusicURLResolveStrategyPrivate()
	{
		if(timer_->isActive())
			timer_->stop();
		if(loop_->isRunning())
			loop_->quit();

		delete nam_;
#ifdef DEBUG_OUTPUT
		qDebug() << "~YandexMusicURLResolveStrategyPrivate()";
#endif
	}

	QUrl getUrl()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicURLResolveStrategyPrivate::getUrl()";
#endif
		//tracks/{track_id}/download-info
		QUrl url = QUrl(QString("%1/tracks/%2/download-info").arg(apiURL, tune_->url), QUrl::StrictMode);
		QNetworkRequest nr(url);
		YandexMusicOauth::setupRequest(&nr);
		QNetworkReply *reply = nam_->get(nr);
		connect(reply, &QNetworkReply::finished, this, &YandexMusicURLResolveStrategyPrivate::tuneUrlFinishedStepOne);
		connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &YandexMusicURLResolveStrategyPrivate::requestError);
		timer_->start();
		loop_->exec();
		if(timer_->isActive())
			timer_->stop();
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicURLResolveStrategyPrivate::getUrl()  finished";
#endif
		return url_;
	}

private slots:
	void tuneUrlFinishedStepOne()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicURLResolveStrategyPrivate::tuneUrlFinishedStepOne()";
#endif
		QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
		reply->deleteLater();
		if(reply->error() == QNetworkReply::NoError) {
			QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
			QJsonObject jo = doc.object();

			if(jo.contains("result")) {
				auto arr = jo.value("result").toArray();
				if(arr.count() > 0) {
					QString u = arr.first().toObject().value("downloadInfoUrl").toString();
					QUrl url(u, QUrl::StrictMode);
					QNetworkRequest nr(url);
					YandexMusicOauth::setupRequest(&nr);
					QNetworkReply *reply = nam_->get(nr);
					connect(reply, &QNetworkReply::finished, this, &YandexMusicURLResolveStrategyPrivate::tuneUrlFinishedStepTwo);
					connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &YandexMusicURLResolveStrategyPrivate::requestError);
				}
			}
		}
		else {
			loop_->quit();
			qDebug() << reply->errorString();
		}
	}

	void tuneUrlFinishedStepTwo()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicURLResolveStrategyPrivate::tuneUrlFinishedStepTwo()";
#endif
		QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
		reply->deleteLater();
		if(reply->error() == QNetworkReply::NoError) {
			QDomDocument doc;
			doc.setContent(reply->readAll());
			auto elem = doc.documentElement();
			if(elem.hasChildNodes()) {
				const QString host	= elem.firstChildElement("host").text();
				const QString path	= elem.firstChildElement("path").text();
				const QString ts	= elem.firstChildElement("ts").text();
				const QString s		= elem.firstChildElement("s").text();
//				const QString region	= elem.firstChildElement("region").text();
//				const QString id = reply->property("id").toString();

				QByteArray md5 = QCryptographicHash::hash("XGRlBW9FXlekgbPrRHuSiA" + (path.right(path.length() - 1) + s).toLatin1(), QCryptographicHash::Md5);

				url_ = QUrl(QString("https://%1/get-mp3/%2/%3%4") //?track-id=%5&region=%6&from=service-search
					    .arg(host)
					    .arg(QString(md5.toHex()))
					    .arg(ts)
					    .arg(path)
					    //					    .arg(id)
					    //					    .arg(region)
					    );
			}
		}
		else {
			qDebug() << reply->errorString();
		}
		loop_->quit();
	}

	void requestError(QNetworkReply::NetworkError /*error*/)
	{
		QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
		reply->deleteLater();
		loop_->quit();
	}

private:
	QUrl url_;
	QEventLoop* loop_;
	QTimer* timer_;
	Tune *tune_;
	QNetworkAccessManager* nam_;
};



YandexMusicURLResolveStrategy *YandexMusicURLResolveStrategy::instance()
{
	if(!instance_)
		instance_ = new YandexMusicURLResolveStrategy();

	return instance_;
}

YandexMusicURLResolveStrategy::~YandexMusicURLResolveStrategy()
{
	delete mutex_;
}

void YandexMusicURLResolveStrategy::reset()
{
	delete instance_;
	instance_ = 0;
}

YandexMusicURLResolveStrategy::YandexMusicURLResolveStrategy() :
	TuneURLResolveStrategy(QCoreApplication::instance()),
	mutex_(new QMutex)
{
}

QUrl YandexMusicURLResolveStrategy::getUrl(const Tune *t)
{
	QMutexLocker l(mutex_);
	YandexMusicURLResolveStrategyPrivate p(t);
	return p.getUrl();
}

QString YandexMusicURLResolveStrategy::name() const
{
	return "Yandex.Music";
}


YandexMusicURLResolveStrategy* YandexMusicURLResolveStrategy::instance_ = 0;

#include "yandexmusicurlresolvestrategy.moc"
