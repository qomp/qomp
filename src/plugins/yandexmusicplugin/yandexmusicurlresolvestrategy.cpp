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

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QMutexLocker>
#include <QMutex>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QTimer>


static const int TimerInterval = 5000;

YandexMusicURLResolveStrategy *YandexMusicURLResolveStrategy::instance()
{
	if(!instance_)
		instance_ = new YandexMusicURLResolveStrategy();

	return instance_;
}

YandexMusicURLResolveStrategy::YandexMusicURLResolveStrategy() :
	QObject(QCoreApplication::instance()),
	loop_(new QEventLoop(this)),
	mutex_(new QMutex),
	timer_(new QTimer(this))
{
	timer_->setSingleShot(true);
	timer_->setInterval(TimerInterval);
	connect(timer_, SIGNAL(timeout()), SLOT(timeout()));
}

YandexMusicURLResolveStrategy::~YandexMusicURLResolveStrategy()
{
	delete mutex_;
}

QUrl YandexMusicURLResolveStrategy::getUrl(const Tune *t)
{
	QMutexLocker locker(mutex_);

	url_ = QUrl();
	tune_ = (Tune*)t;

	QUrl url = QUrl(QString("http://storage.music.yandex.ru/get/%1/2.xml").arg(tune_->url), QUrl::StrictMode);
	QNetworkRequest nr(url);
	nr.setRawHeader("Accept", "*/*");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QNetworkReply *reply = QompNetworkingFactory::instance()->getNetworkAccessManager()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinishedStepOne()));
	timer_->start();
	loop_->exec();

	return url_;
}

QString YandexMusicURLResolveStrategy::name() const
{
	return "Yandex.Music";
}

void YandexMusicURLResolveStrategy::tuneUrlFinishedStepOne()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		const QString replyStr = QString::fromUtf8(reply->readAll());
		QRegExp re("<track filename=\"([^\"]+)\"");
		if(re.indexIn(replyStr) != -1) {
			QString fileName = re.cap(1);
			QUrl url(QString("http://storage.music.yandex.ru/download-info/%1/%2").arg(tune_->url, fileName));
			QNetworkRequest nr(url);
			nr.setRawHeader("Accept", "*/*");
			nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
			QNetworkReply *reply = QompNetworkingFactory::instance()->getNetworkAccessManager()->get(nr);
			connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinishedStepTwo()));
		}
	}
	else {
		loop_->exit();
	}
}

void YandexMusicURLResolveStrategy::tuneUrlFinishedStepTwo()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString replyStr = QString::fromUtf8(reply->readAll());
		QRegExp re("<host>([^<]+)</host><path>([^<]+)</path><ts>([^<]+)</ts><region>([^<]+)</region><s>([^<]+)</s>");
		if(re.indexIn(replyStr) != -1) {
			const QString host = re.cap(1);
			const QString path = re.cap(2);
			const QString ts = re.cap(3);
			const QString region = re.cap(4);
			QString s = re.cap(5);
			const QString id = reply->property("id").toString();

			QByteArray md5 = QCryptographicHash::hash("XGRlBW9FXlekgbPrRHuSiA" + (path.right(path.length() - 1) + s).toLatin1(), QCryptographicHash::Md5);

			url_ = QUrl(QString("http://%1/get-mp3/%2/%3%4?track-id=%5&region=%6&from=service-search")
					.arg(host)
					.arg(QString(md5.toHex()))
					.arg(ts)
					.arg(path)
					.arg(id)
					.arg(region) );

		}
	}
	loop_->exit();
}

void YandexMusicURLResolveStrategy::timeout()
{
	loop_->exit();
}

YandexMusicURLResolveStrategy* YandexMusicURLResolveStrategy::instance_ = 0;
