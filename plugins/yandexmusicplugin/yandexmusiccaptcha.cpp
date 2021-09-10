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

#include "yandexmusiccaptcha.h"
#include "yandexmusicoauth.h"

#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrlQuery>
#include <qompplugincaptchadialog.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QNetworkReply>

YandexMusicCaptcha::YandexMusicCaptcha(QNetworkAccessManager *nam, QObject *parent) : QObject(parent),
	nam_(nam),
	captchaInProgress_(false)
{

}

YandexMusicCaptcha::CheckResult YandexMusicCaptcha::checkCaptcha(const QUrl& replyUrl, const QByteArray &reply)
{
	QJsonDocument doc = QJsonDocument::fromJson(reply);
	QJsonObject root = doc.object();
	if(!root.contains("type") || root.value("type").toString() != QStringLiteral("captcha"))
		return YandexMusicCaptcha::NoCaptcha;

	QJsonObject captcha = root.value("captcha").toObject();

	const QString status = captcha.value("status").toString();
	if(status == "success")
		return YandexMusicCaptcha::CaptchaSuccess;

	if(captchaInProgress_)
		return YandexMusicCaptcha::InProgress;

	captchaInProgress_ = true;

	const QString imageURL = captcha.value("img-url").toString();
	const QString page = captcha.value("captcha-page").toString();
	const QString ref = QUrl(page).query(QUrl::FullyEncoded);

	QString key = captcha.value("key").toString();

#ifdef DEBUG_OUTPUT
	qDebug() << "YandexMusicCaptcha::checkCaptcha():  " << imageURL <<  key << page << ref;
#endif

	QString key2;
	const QPixmap px = getCaptcha(imageURL, &key2);
	QompPluginCaptchaDialog dlg(px, parent());
	if(dlg.start()) {
		QUrl url(QString("%1://%2/checkcaptcha?key=%3&%4&rep=%5")
			 .arg(replyUrl.scheme(), replyUrl.host(), key, ref,
			      QUrl::toPercentEncoding(dlg.result())), QUrl::StrictMode);
#ifdef DEBUG_OUTPUT
		qDebug() << url.toString(QUrl::FullyEncoded);
#endif
		QNetworkRequest nr(url);
		nr.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
		nr.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
		QNetworkReply *r = nam_->get(nr);
		emit captchaReady(r);
	}

	captchaInProgress_ = false;

	return YandexMusicCaptcha::Resolving;
}

QPixmap YandexMusicCaptcha::getCaptcha(const QString &captchaUrl, QString *key)
{
#ifdef DEBUG_OUTPUT
	qDebug() << "YandexMusicCaptcha::getCaptcha():  " << captchaUrl;
#endif
	QUrl url(captchaUrl);
	QNetworkRequest nr(url);

	YandexMusicOauth::setupRequest(&nr);
	QNetworkReply* r = nam_->get(nr);

	QEventLoop el;
	connect(r, &QNetworkReply::finished, &el, &QEventLoop::quit);
	el.exec();
	r->deleteLater();

	QPixmap pix;
	if (r->error() == QNetworkReply::NoError) {
		if(url.hasQuery()) {
			const QUrlQuery q(url.query());
			foreach(const auto& query, q.queryItems()) {
				if(query.first == "key") {
					*key = query.second;
					break;
				}
			}
		}
		QByteArray ba = r->readAll();
		pix.loadFromData(ba);
	}

	return pix;
}
