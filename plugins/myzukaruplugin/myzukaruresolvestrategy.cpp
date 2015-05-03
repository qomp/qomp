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
#include "tuneurlchecker.h"

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
		connect(timer_, SIGNAL(timeout()), loop_, SLOT(quit()));
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
		QUrl url(QString("%1Song/Play/%2").arg(MYZUKA_URL).arg(tune_->url));
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "*/*");
		nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
		nr.setRawHeader("Referer", MYZUKA_URL);
		QNetworkReply *reply = nam_->get(nr);
		connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinished()));
		connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(requestError()));
		timer_->start();
		loop_->exec();
		if(timer_->isActive())
			timer_->stop();
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::getUrl()  finished";
#endif
		TuneUrlChecker uc(nam_, url_, this);
		if(uc.result())
			return url_;

		return(QUrl());
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
//			QRegExp re("\"(http://[^\"]+)\"");
//			QString text = QString::fromUtf8(reply->readAll());
//			qDebug() << text;
//			if(re.indexIn(text) != -1) {
//				url_.setUrl(re.cap(1).replace("\\u0026", "&"),QUrl::TolerantMode);
//			}
//			else {
				url_.setUrl(reply->header(QNetworkRequest::LocationHeader).toString());
//			}
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


MyzukaruResolveStrategy* MyzukaruResolveStrategy::instance_ = 0;

#include "myzukaruresolvestrategy.moc"
