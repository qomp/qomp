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
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

static const int TimerInterval = 5000;


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
		connect(timer_, SIGNAL(timeout()), loop_, SLOT(quit()));
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
		QUrl url = QUrl(QString("http://storage.music.yandex.ru/get/%1/2.xml").arg(tune_->url), QUrl::StrictMode);
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "*/*");
		nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
		nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
		QNetworkReply *reply = nam_->get(nr);
		connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinishedStepOne()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(requestError()));
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
			const QString replyStr = QString::fromUtf8(reply->readAll());
			QRegExp re("<track filename=\"([^\"]+)\"");
			if(re.indexIn(replyStr) != -1) {
				QString fileName = re.cap(1);
				QUrl url(QString("http://storage.music.yandex.ru/download-info/%1/%2").arg(tune_->url, fileName));
				QNetworkRequest nr(url);
				nr.setRawHeader("Accept", "*/*");
				nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
				nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
				QNetworkReply *reply = nam_->get(nr);
				connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinishedStepTwo()));
				connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(requestError()));
			}
		}
		else {
			loop_->quit();
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
		loop_->quit();
	}

	void requestError()
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
