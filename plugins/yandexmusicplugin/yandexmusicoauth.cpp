/*
 * Copyright (C) 2021  Khryukin Evgeny
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

#include "yandexmusicoauth.h"
#include "options.h"
#include "qompnetworkingfactory.h"
#include "yandexmusicurlresolvestrategy.h"
#include "yandexmusiccaptcha.h"

#include <QUrlQuery>
#include <QUrl>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

static const QString  optionYaToken = "plugins.yandex_music.ya_token";
static const QString  optionYaTokenTtl = "plugins.yandex_music.ya_token_ttl";
static const QString  optionYaUserInfo = "plugins.yandex_music.user_info";
static const QString  optionYaUserName = "plugins.yandex_music.user_name";

//can't use this. sad
static const QString qompID = "371640f2734e43a4a0ab949c607a1a01";
static const QString qompPass = "68d3f871e4444d3babf9ff2629dbe115";

//yandex music app:
#define TOKEN_CLIENT_ID "23cabbbdc6cd418abb4b39c32c41195d"
#define TOKEN_CLIENT_SECRET "53bc75238f0c4d08a118e51fe9203300"

static const QString qompTokenUrl = "https://oauth.yandex.ru/token";


YandexMusicOauth::YandexMusicOauth(QObject *parent) : QObject(parent),
	_nam(QompNetworkingFactory::instance()->getThreadedNAM()),
	captcha_(new YandexMusicCaptcha(_nam, this))
{
	connect(captcha_, &YandexMusicCaptcha::captchaReady, [&](QNetworkReply *r) {
		connect(r, &QNetworkReply::finished, this, &YandexMusicOauth::grantRequestFinished);
	});
}

YandexMusicOauth::~YandexMusicOauth()
{
	delete _nam;
}

QString YandexMusicOauth::userName()
{
	return Options::instance()->getOption(optionYaUserName).toString();
}

QString YandexMusicOauth::token()
{
	return Options::instance()->getOption(optionYaToken).toString();
}

QDateTime YandexMusicOauth::tokenTtl()
{
	return Options::instance()->getOption(optionYaTokenTtl).toDateTime();
}

void YandexMusicOauth::setupRequest(QNetworkRequest *nr)
{
	nr->setRawHeader("Accept", "*/*");
	nr->setRawHeader("X-Requested-With", "XMLHttpRequest");
	nr->setHeader(QNetworkRequest::UserAgentHeader, "Yandex-Music-API");
	nr->setRawHeader("X-Yandex-Music-Client", "YandexMusicAndroid/23020251");
#ifndef HAVE_QT6
	nr->setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#endif

	const QString token = YandexMusicOauth::token();
	if(token.size() > 0) {
		nr->setRawHeader("Authorization", QString("OAuth %1").arg(token).toLatin1());
	}
}

void YandexMusicOauth::grant(const QString &user, const QString &password)
{
	QUrl url(qompTokenUrl);

	QUrlQuery q;
	q.addQueryItem("grant_type", "password");
	q.addQueryItem("username", user);
	q.addQueryItem("password", password);
	q.addQueryItem("client_id", TOKEN_CLIENT_ID);
	q.addQueryItem("client_secret", TOKEN_CLIENT_SECRET);

	Options::instance()->setOption(optionYaToken, QVariant::Invalid);

	QNetworkRequest r(url);
	r.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	YandexMusicOauth::setupRequest(&r);

	QNetworkReply *reply = _nam->post(r, q.toString().toLatin1());
	connect(reply, &QNetworkReply::finished, this, &YandexMusicOauth::grantRequestFinished);

	Options::instance()->setOption(optionYaUserName, user);
}

void YandexMusicOauth::grantRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	reply->deleteLater();

	if(reply->error() == QNetworkReply::NoError) {
		const QByteArray info = reply->readAll();
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicOauth::grantRequestFinished()" << info;
#endif
		auto res = captcha_->checkCaptcha(reply->url(), info);
		if(res != YandexMusicCaptcha::NoCaptcha)  {
			return;
		}

		QJsonDocument doc = QJsonDocument::fromJson(info);
		QJsonObject jo = doc.object();
		if(jo.contains("access_token")) {
			Options::instance()->setOption(optionYaUserInfo, info);

			Options::instance()->setOption(optionYaToken, jo.value("access_token").toString());
			QDateTime exp = QDateTime::currentDateTime().addSecs(jo.value("expires_in").toInt());
			Options::instance()->setOption(optionYaTokenTtl, exp);
//			"{\"access_token\": \"\", \"expires_in\": 31536000, \"token_type\": \"bearer\", \"uid\": 1111111}"

			emit granted();
		}
		else
			emit requestError(tr("Invalid response"));
	}
	else {
#ifdef DEBUG_OUTPUT
		qDebug() << "YandexMusicOauth::grantRequestFinished()" << reply->errorString();
#endif
		emit requestError(reply->errorString());
	}
}
