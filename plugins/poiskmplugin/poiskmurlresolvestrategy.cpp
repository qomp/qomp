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

#include "poiskmurlresolvestrategy.h"
#include "qompnetworkingfactory.h"
#include "poiskmplugindefines.h"

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
#include <QSharedPointer>

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

static const int TimerInterval = 5000;

class PoiskmURLResolveStrategyPrivate : public QObject
{
	Q_OBJECT
public:
	explicit PoiskmURLResolveStrategyPrivate(const Tune* t) :
		QObject(),
		loop_(new QEventLoop(this)),
		timer_(new QTimer(this)),
		tune_(const_cast<Tune*>(t)),
		nam_(QompNetworkingFactory::instance()->getThreadedNAM())
	{
		timer_->setSingleShot(true);
		timer_->setInterval(TimerInterval);
		connect(timer_, SIGNAL(timeout()), loop_, SLOT(quit()));
	}

	~PoiskmURLResolveStrategyPrivate()
	{
		if(timer_->isActive())
			timer_->stop();
		if(loop_->isRunning())
			loop_->quit();
#ifdef DEBUG_OUTPUT
		qDebug() << "~PoiskmURLResolveStrategyPrivate()";
#endif
	}

	QUrl getUrl()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "PoiskmURLResolveStrategyPrivate::getUrl()";
#endif
		QUrl url(QString("https://%1/?do=getById&id=%2").arg(POISKM_PLUGIN_URL).arg(tune_->url));
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "*/*");
		nr.setRawHeader("Referer", QString("https://%1").arg(POISKM_PLUGIN_URL).toLatin1());
		nr.setRawHeader("Host", POISKM_PLUGIN_URL);
		nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, false);
		QNetworkReply *reply = nam_->get(nr);
		reply->setParent(nam_.data());
		connect(reply, &QNetworkReply::finished, this, &PoiskmURLResolveStrategyPrivate::tuneUrlFinished);
		timer_->start();
		loop_->exec();
		if(timer_->isActive())
			timer_->stop();
#ifdef DEBUG_OUTPUT
		qDebug() << "PoiskmURLResolveStrategyPrivate::getUrl()  finished";
#endif
		return url_;
	}

private slots:
	void tuneUrlFinished()
	{
#ifdef DEBUG_OUTPUT
		qDebug() << "PoiskmURLResolveStrategyPrivate::tuneUrlFinished()";
#endif
		QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
		reply->deleteLater();
		if(reply->error() == QNetworkReply::NoError) {
			url_ = reply->header(QNetworkRequest::LocationHeader).toString();
		}

		loop_->quit();
	}

private:
	QUrl url_;
	QEventLoop* loop_;
	QTimer* timer_;
	Tune *tune_;
	QSharedPointer<QNetworkAccessManager> nam_;
};



PoiskmURLResolveStrategy *PoiskmURLResolveStrategy::instance()
{
	if(!instance_)
		instance_ = new PoiskmURLResolveStrategy();

	return instance_;
}

PoiskmURLResolveStrategy::~PoiskmURLResolveStrategy()
{
	delete mutex_;
}

void PoiskmURLResolveStrategy::reset()
{
	delete instance_;
	instance_ = nullptr;
}

PoiskmURLResolveStrategy::PoiskmURLResolveStrategy() :
	TuneURLResolveStrategy(QCoreApplication::instance()),
	mutex_(new QMutex)
{
}

QUrl PoiskmURLResolveStrategy::getUrl(const Tune *t)
{
	QMutexLocker l(mutex_);
	PoiskmURLResolveStrategyPrivate p(t);
	return p.getUrl();
}

QString PoiskmURLResolveStrategy::name() const
{
	return "Poiskm";
}


PoiskmURLResolveStrategy* PoiskmURLResolveStrategy::instance_ = nullptr;

#include "poiskmurlresolvestrategy.moc"
