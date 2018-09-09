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

#ifndef QOMPNETWORKINGFACTORY_H
#define QOMPNETWORKINGFACTORY_H

#include <QObject>
#include "libqomp_global.h"

class QNetworkAccessManager;
class QNetworkProxy;
class QNetworkReply;

class LIBQOMPSHARED_EXPORT QompNetworkingFactory : public QObject
{
	Q_OBJECT
public:
	static QompNetworkingFactory* instance();
	~QompNetworkingFactory();

	void updateProxySettings() const;

	/**
	 * @brief getMainNAM
	 * @return main application's QNetworkAccessManager
	 * Clients should never delete it!
	 */
	QNetworkAccessManager* getMainNAM() const;

	/**
	 * @brief getThreadedNAM
	 * @return QNetworkAccessManager that could be used at threads
	 * Client should delete it when it's not needed any more
	 */
	QNetworkAccessManager* getThreadedNAM();

	bool isNetworkAvailable() const;

private slots:
	void logEvent(QNetworkReply* reply);
	
private:
	QompNetworkingFactory();
	QNetworkProxy getProxy() const;

	static QompNetworkingFactory* instance_;

	QNetworkAccessManager* manager_;
};

#endif // QOMPNETWORKINGFACTORY_H
