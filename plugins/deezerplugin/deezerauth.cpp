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

#include "deezerauth.h"
#include "qompnetworkingfactory.h"
#include "deezerplugindefines.h"
#include "options.h"
#include "common.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

DeezerAuth::DeezerAuth(QObject *parent) :
	QObject(parent),
	status_(false)
{
	token_ = Qomp::decodePassword(Options::instance()->getOption(DEEZER_TOKEN_OPTION).toString(), DEEZER_TOKEN_OPTION);
}

DeezerAuth::~DeezerAuth()
{
	Options::instance()->setOption(DEEZER_TOKEN_OPTION, Qomp::encodePassword(token_, DEEZER_TOKEN_OPTION));
}

void DeezerAuth::start()
{
	QString url = QString("http://connect.deezer.com/en/login.php?format=popup&response_type=token&perms=basic_access%1")
			.arg(DEEZER_APP_ID);
	QNetworkRequest nr(url);
	QNetworkReply* reply = QompNetworkingFactory::instance()->getMainNAM()->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(authFinished()));
}

void DeezerAuth::authFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();


	if(reply->error() == QNetworkReply::NoError) {
		const QString res = QString::fromUtf8(reply->readAll());
#ifdef DEBUG_OUTPUT
		qDebug() << res;
		qDebug() << reply->url();
#endif
		status_ = true;
	}
}
