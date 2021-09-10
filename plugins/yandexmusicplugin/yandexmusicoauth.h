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

#ifndef YANDEXMUSICOAUTH_H
#define YANDEXMUSICOAUTH_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkRequest;
class YandexMusicCaptcha;

class YandexMusicOauth : public QObject
{
	Q_OBJECT
public:
	explicit YandexMusicOauth(QObject *parent = nullptr);
	virtual ~YandexMusicOauth();
	QString userName();

	static QString token();
	static QDateTime tokenTtl();

	static void setupRequest(QNetworkRequest *nr);

public slots:
	void grant(const QString& user, const QString& password);

signals:
	void granted();
	void requestError(const QString&);

private:
	void setupHandler();
	void updateToken();

private slots:
	void grantRequestFinished();

private:
	QNetworkAccessManager *_nam;
	YandexMusicCaptcha *captcha_;

};

#endif // YANDEXMUSICOAUTH_H
