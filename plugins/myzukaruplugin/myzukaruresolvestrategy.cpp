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

#include "myzukaruresolvestrategy.h"
#include "qompnetworkingfactory.h"
#include "myzukarudefines.h"
#include "common.h"

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
static const int ContentNotFoundResult = -1;
static const int PageError = -2;
static const int NoRequestError = 0;

static const QString songsRegExp = QString(
			"<div\\s+id=\"playerDiv[^>]+>\\s+"
			"<div\\s+id=\"[^\"]+\"\\s+class=\"play[^>]+>\\s+"
			"<span\\s+class=\"[^\"]+\"\\s+data-url=\"/Song/Play/([^\"]+)\""		//cap(1) - url
			);


class MyzukaruResolveStrategyPrivate : public QObject
{
	Q_OBJECT
public:
	explicit MyzukaruResolveStrategyPrivate(const Tune* t) :
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

	~MyzukaruResolveStrategyPrivate()
	{
		if(timer_->isActive())
			timer_->stop();
		if(loop_->isRunning())
			loop_->quit();

		delete nam_;
#ifdef DEBUG_OUTPUT
		qDebug() << "~MyzukaruResolveStrategyPrivate()";
#endif
	}

	QUrl getUrl()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::getUrl()";
#endif

		int ret = getDirectUrl();
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::getUrl()  finished";
#endif
		if(ret == ContentNotFoundResult) {
			ret = getBaseUrl();
			if(ret == NoRequestError) {
				tune_->url = url_.toString();
				url_.clear();
				getDirectUrl();
			}
		}

		return url_;
	}

	int startLoop()
	{
		timer_->start();
		int ret = loop_->exec();
		if(timer_->isActive())
			timer_->stop();

		return ret;
	}

	int getDirectUrl()
	{
		QUrl url(QString("%1Song/Play/%2").arg(MYZUKA_URL).arg(tune_->url));
		QNetworkRequest nr(createRequest(url));
		QNetworkReply *reply = nam_->get(nr);
		connect(reply, &QNetworkReply::finished, this, &MyzukaruResolveStrategyPrivate::tuneUrlFinished);
		return startLoop();
	}

	int getBaseUrl()
	{
		QUrl url(QUrl::fromPercentEncoding(tune_->directUrl.toLatin1()));
		QNetworkRequest nr(createRequest(url));
		QNetworkReply *reply = nam_->get(nr);
		connect(reply, &QNetworkReply::finished, this, &MyzukaruResolveStrategyPrivate::tunePageFinished);
		return startLoop();
	}

	QNetworkRequest createRequest(const QUrl& url) const
	{
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "*/*");
		nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
		nr.setRawHeader("Referer", MYZUKA_URL);
		return nr;
	}

private slots:
	void tuneUrlFinished()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::tuneUrlFinished()";
#endif
		QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
		reply->deleteLater();
		if(reply->error() == QNetworkReply::NoError) {
			url_.setUrl(reply->header(QNetworkRequest::LocationHeader).toString());
		}
		else {
#ifdef DEBUG_OUTPUT
			qDebug() << "reply error: " << reply->error() << reply->errorString();
#endif
			if(reply->error() == QNetworkReply::ContentNotFoundError) {
				loop_->exit(ContentNotFoundResult);
				return;
			}

		}
		loop_->quit();
	}

	void tunePageFinished()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::tunePageFinished()";
#endif
		QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
		reply->deleteLater();
		if(reply->error() == QNetworkReply::NoError) {
			QRegExp re(songsRegExp);
			re.setMinimal(true);
			const QString replyStr = QString::fromUtf8(reply->readAll());
			if(re.indexIn(replyStr) != -1) {
				url_.setUrl(Qomp::unescape(re.cap(1)));
			}
		}
		else {
#ifdef DEBUG_OUTPUT
			qDebug() << "reply error: " << reply->error() << reply->errorString();
#endif
			loop_->exit(PageError);
			return;
		}
		loop_->quit();
	}

public:
	QUrl url_;
	QEventLoop* loop_;
	QTimer* timer_;
	Tune *tune_;
	QNetworkAccessManager* nam_;
};



MyzukaruResolveStrategy *MyzukaruResolveStrategy::instance()
{
	if(!instance_)
		instance_ = new MyzukaruResolveStrategy();

	return instance_;
}

MyzukaruResolveStrategy::~MyzukaruResolveStrategy()
{
	delete mutex_;
}

void MyzukaruResolveStrategy::reset()
{
	delete instance_;
	instance_ = 0;
}

MyzukaruResolveStrategy::MyzukaruResolveStrategy() :
	TuneURLResolveStrategy(QCoreApplication::instance()),
	mutex_(new QMutex)
{
}

QUrl MyzukaruResolveStrategy::getUrl(const Tune *t)
{
	QMutexLocker l(mutex_);
	MyzukaruResolveStrategyPrivate p(t);
	return p.getUrl();
}

QString MyzukaruResolveStrategy::name() const
{
	return "Myzuka.ru";
}

QUrl MyzukaruResolveStrategy::getBaseUrl(const Tune *t)
{
//	QMutexLocker l(mutex_);
	MyzukaruResolveStrategyPrivate p(t);
	if(p.getBaseUrl() == NoRequestError)
		return p.url_;

	return QUrl();
}


MyzukaruResolveStrategy* MyzukaruResolveStrategy::instance_ = 0;

#include "myzukaruresolvestrategy.moc"
