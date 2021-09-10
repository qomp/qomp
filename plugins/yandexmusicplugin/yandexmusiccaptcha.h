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

#ifndef YANDEXMUSICCAPTCHA_H
#define YANDEXMUSICCAPTCHA_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class YandexMusicCaptcha : public QObject
{
	Q_OBJECT
public:
	explicit YandexMusicCaptcha(QNetworkAccessManager *nam, QObject *parent = nullptr);

	enum CheckResult {InProgress, NoCaptcha, CaptchaSuccess, Resolving};

	CheckResult checkCaptcha(const QUrl& replyUrl, const QByteArray& reply);
	QPixmap getCaptcha(const QString& captchaUrl, QString* key);

signals:
	void captchaReady(QNetworkReply* r);
private:
	QNetworkAccessManager *nam_;
	bool captchaInProgress_;

};

#endif // YANDEXMUSICCAPTCHA_H
