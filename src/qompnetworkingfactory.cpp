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
#ifdef HAVE_QT5
#include <QNetworkCookie>
#endif
#include <QFile>


static const QString cookieCache = "/qomp-cookies";

class QompNetworkCookieJar : public QNetworkCookieJar
{
	Q_OBJECT
public:
	QompNetworkCookieJar(QObject* p) : QNetworkCookieJar(p)
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
	manager_(0)
{
	updateProxySettings();
}

void QompNetworkingFactory::checkNAM() const
{
	if(!manager_) {
		manager_ = new QNetworkAccessManager();
		manager_->setCookieJar(new QompNetworkCookieJar(manager_));
	}
}


QompNetworkingFactory* QompNetworkingFactory::instance()
{
	if(!instance_)
		instance_ = new QompNetworkingFactory;

	return instance_;
}

QompNetworkingFactory::~QompNetworkingFactory()
{
	if(manager_)
		delete manager_;
}

void QompNetworkingFactory::updateProxySettings() const
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

	checkNAM();
	manager_->setProxy(proxy);
	QNetworkProxy::setApplicationProxy(proxy);
}

QNetworkAccessManager *QompNetworkingFactory::getNetworkAccessManager() const
{
	updateProxySettings();
	return manager_;
}

bool QompNetworkingFactory::isNetworkAvailable() const
{
	return manager_->networkAccessible() == QNetworkAccessManager::Accessible;
}


#include "qompnetworkingfactory.moc"
