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

#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QCoreApplication>

QompNetworkingFactory* QompNetworkingFactory::instance_ = 0;

QompNetworkingFactory::QompNetworkingFactory() :
	QObject(QCoreApplication::instance()),
	manager_(new QNetworkAccessManager(this))
{
	updateProxySettings();
}


QompNetworkingFactory* QompNetworkingFactory::instance()
{
	if(!instance_)
		instance_ = new QompNetworkingFactory;

	return instance_;
}

void QompNetworkingFactory::updateProxySettings()
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

	manager_->setProxy(proxy);
	QNetworkProxy::setApplicationProxy(proxy);
}

QNetworkAccessManager *QompNetworkingFactory::getNetworkAccessManager() const
{
	return manager_;
}
