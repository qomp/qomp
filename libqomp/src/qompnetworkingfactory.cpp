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

#include "qompnetworkingfactory.h"
#include "defines.h"
#include "options.h"
#include "common.h"

#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QCoreApplication>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QFile>


static const QString cookieCache = "/qomp-cookies";

class QompNetworkCookieJar : public QNetworkCookieJar
{
	Q_OBJECT
public:
	explicit QompNetworkCookieJar(QObject* p) : QNetworkCookieJar(p)
	{
		QFile file(Qomp::cacheDir()+ cookieCache);
		if(file.open(QFile::ReadOnly)) {
			QList<QNetworkCookie> list;
			while(!file.atEnd()) {
				QByteArray ba = file.readLine();
				list.append(QNetworkCookie::parseCookies(ba));
			}
			setAllCookies(list);
		}
	}

	~QompNetworkCookieJar()
	{
		saveCookies();
	}

	void saveCookies()
	{
		QFile file(Qomp::cacheDir()+ cookieCache);
		if(file.open(QFile::WriteOnly | QFile::Truncate)) {
			foreach(const QNetworkCookie& cookie, allCookies()) {
				file.write(cookie.toRawForm());
				file.putChar('\n');
			}
		}
	}
};


QompNetworkingFactory* QompNetworkingFactory::instance_ = 0;

QompNetworkingFactory::QompNetworkingFactory() :
	QObject(QCoreApplication::instance()),
	manager_(new QNetworkAccessManager)
{
	manager_->setCookieJar(new QompNetworkCookieJar(manager_));
	connect(manager_, &QNetworkAccessManager::finished, this, &QompNetworkingFactory::logEvent);
	updateProxySettings();
}

QNetworkProxy QompNetworkingFactory::getProxy() const
{
	QNetworkProxy proxy;
	if(Options::instance()->getOption(OPTION_PROXY_USE).toBool()) {
		proxy.setHostName(Options::instance()->getOption(OPTION_PROXY_HOST).toString());
		proxy.setPort(Options::instance()->getOption(OPTION_PROXY_PORT).toInt());
		proxy.setUser(Options::instance()->getOption(OPTION_PROXY_USER).toString());
		proxy.setPassword(Options::instance()->getOption(OPTION_PROXY_PASS).toString());

		if(Options::instance()->getOption(OPTION_PROXY_TYPE) == "SOCKS5")
			proxy.setType(QNetworkProxy::Socks5Proxy);
		else
			proxy.setType(QNetworkProxy::HttpCachingProxy);


	}
	return proxy;
}

QompNetworkingFactory* QompNetworkingFactory::instance()
{
	if(!instance_)
		instance_ = new QompNetworkingFactory;

	return instance_;
}

QompNetworkingFactory::~QompNetworkingFactory()
{
	delete manager_;
}

void QompNetworkingFactory::updateProxySettings() const
{
	QNetworkProxy p = getProxy();
	manager_->setProxy(p);
	QNetworkProxy::setApplicationProxy(p);
}

QNetworkAccessManager *QompNetworkingFactory::getMainNAM() const
{
	return manager_;
}

QNetworkAccessManager *QompNetworkingFactory::getThreadedNAM()
{
	static_cast<QompNetworkCookieJar*>(manager_->cookieJar())->saveCookies();
	QNetworkAccessManager* m = new QNetworkAccessManager;
	m->setCookieJar(new QompNetworkCookieJar(m));
	m->setProxy(getProxy());
	connect(manager_, &QNetworkAccessManager::finished, this, &QompNetworkingFactory::logEvent, Qt::DirectConnection);
	return m;
}

bool QompNetworkingFactory::isNetworkAvailable() const
{
	return manager_->networkAccessible() == QNetworkAccessManager::Accessible;
}

void QompNetworkingFactory::logEvent(QNetworkReply *reply)
{
	const QUrl u = reply->url();
	Qomp::logEvent(NETWORK_EVENT, {{NETWORK_EVENT_DATA, u.host(QUrl::PrettyDecoded)}});
}


#include "qompnetworkingfactory.moc"
