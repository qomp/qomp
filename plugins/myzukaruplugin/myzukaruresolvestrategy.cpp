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

#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegExp>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QTimer>


#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

static const int TimerInterval = 5000;


class MyzukaruResolveStrategyPrivate : public QObject
{
	Q_OBJECT
public:
	MyzukaruResolveStrategyPrivate(const Tune* t) :
		QObject(),
		loop_(new QEventLoop(this)),
		timer_(new QTimer(this)),
		tune_((Tune*)t)
	{
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
#ifdef DEBUG_OUTPUT
		qDebug() << "~MyzukaruResolveStrategyPrivate()";
#endif
	}

	QUrl getUrl()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::getUrl()";
#endif
		QUrl url(MYZUKA_URL);
		url.setPath(QString("/Song/GetFileUrl/%1").arg(tune_->url));
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "*/*");
		nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
		QNetworkReply *reply = QompNetworkingFactory::instance()->getNetworkAccessManager()->get(nr);
		connect(reply, SIGNAL(finished()), SLOT(tuneUrlFinished()));
		timer_->start();
		loop_->exec();
		if(timer_->isActive())
			timer_->stop();
#ifdef DEBUG_OUTPUT
		qDebug() << "MyzukaruResolveStrategyPrivate::getUrl()  finished";
#endif
		deleteLater();
		return url_;
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
			QRegExp re("\"(http://[^\"]+)\"");
			QString text = QString::fromUtf8(reply->readAll());
			if(re.indexIn(text) != -1) {
				url_ = re.cap(1);
			}
		}
		loop_->quit();
	}

private:
	QUrl url_;
	QEventLoop* loop_;
	QTimer* timer_;
	Tune *tune_;
};



MyzukaruResolveStrategy *MyzukaruResolveStrategy::instance()
{
	if(!instance_)
		instance_ = new MyzukaruResolveStrategy();

	return instance_;
}

void MyzukaruResolveStrategy::reset()
{
	delete instance_;
	instance_ = 0;
}

MyzukaruResolveStrategy::MyzukaruResolveStrategy() :
	TuneURLResolveStrategy(QCoreApplication::instance())
{
}

QUrl MyzukaruResolveStrategy::getUrl(const Tune *t)
{
	MyzukaruResolveStrategyPrivate* p = new MyzukaruResolveStrategyPrivate(t);
	return p->getUrl();
}

QString MyzukaruResolveStrategy::name() const
{
	return "Myzuka.ru";
}


MyzukaruResolveStrategy* MyzukaruResolveStrategy::instance_ = 0;

#include "myzukaruresolvestrategy.moc"
