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

#include "tuneurlchecker.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const int TIMEOUT = 5000;


TuneUrlChecker::TuneUrlChecker(QNetworkAccessManager *nam, const QUrl &url, QObject *parent) :
	QObject(parent),
	nam_(nam),
	result_(false),
	loop_(new QEventLoop(this)),
	url_(url),
	timer_(new QTimer(this)),
	reply_(nullptr)
{
	if(url_.toString().right(1) == ";") {//SHOUTcast server
		QString u = url_.toString();
		u.chop(1);
		url_.setUrl(u);
	}

	timer_->setSingleShot(true);
	timer_->setInterval(TIMEOUT);
	connect(timer_, SIGNAL(timeout()), SLOT(timeout()));
}

bool TuneUrlChecker::result() const
{
#ifdef DEBUG_OUTPUT
	qDebug() << "TuneUrlChecker::result()" << "start check";
#endif
	QNetworkRequest nr(url_);
	nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
	reply_ = nam_->get(nr);
	connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(error()));
	connect(reply_, SIGNAL(readyRead()), SLOT(readyRead()));
	connect(reply_, SIGNAL(destroyed()), loop_, SLOT(quit()));
	timer_->start();

	loop_->exec();
#ifdef DEBUG_OUTPUT
	qDebug() << "TuneUrlChecker::result()" << "finish check" << result_;
#endif
	return result_;
}

void TuneUrlChecker::readyRead()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "TuneUrlChecker::readyRead()";
#endif
	timer_->stop();
	reply_->abort();
	reply_->deleteLater();
	result_ = true;
}

void TuneUrlChecker::error()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "TuneUrlChecker::error()" << reply_->errorString();
#endif
	timer_->stop();
	reply_->deleteLater();
}

void TuneUrlChecker::timeout()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "TuneUrlChecker::timeout()";
#endif
	reply_->abort();
	reply_->deleteLater();
}
