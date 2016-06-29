/*
 * Copyright (C) 2016  Khryukin Evgeny
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

#include "qompinstancewatcher.h"
#include "options.h"
#include "defines.h"

#include <QBuffer>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLocalSocket>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const QString SYS_KEY = "qomp-instance";
static int CONNECTION_INTERVAL = 100;
static const QString COMMAND = "command";
static const QString ARG = "arg";
static const QString COM_SHOW = "show";
static const QString COM_TUNE = "tune";
static const QString VAL_URL  = "url";

QompInstanceWatcher::QompInstanceWatcher(QObject *parent) :
	QObject(parent),
	_serv(this),
	_firstInstance(true)
{	
	connect(&_serv, &QLocalServer::newConnection, this, &QompInstanceWatcher::newConnection);
	_firstInstance = !checkServerExists();
	setupServer();
}

QompInstanceWatcher::~QompInstanceWatcher()
{

}

bool QompInstanceWatcher::newInstanceAllowed() const
{
	return _firstInstance || !Options::instance()->getOption(OPTION_ONE_COPY).toBool();
}

void QompInstanceWatcher::sendCommandShow()
{
	QJsonObject obj;
	obj.insert(COMMAND, QJsonValue(COM_SHOW));
	QJsonDocument doc(obj);
	sendCommand(doc);
}

void QompInstanceWatcher::sendCommandTune(const QString &url)
{
	QJsonObject obj;
	obj.insert(COMMAND, QJsonValue(COM_TUNE));
	obj.insert(VAL_URL, QJsonValue(url));
	QJsonDocument doc(obj);
	sendCommand(doc);
}

void QompInstanceWatcher::newConnection()
{
#ifdef DEBUG_OUTPUT
	qDebug() << "QompInstanceWatcher::newConnection";
#endif
	QLocalSocket* soc = _serv.nextPendingConnection();
	connect(soc, &QLocalSocket::readChannelFinished, this, &QompInstanceWatcher::dataReady);
	connect(soc, &QLocalSocket::disconnected, soc, &QLocalSocket::deleteLater);
}

void QompInstanceWatcher::dataReady()
{
	QLocalSocket* soc = static_cast<QLocalSocket*>(sender());

	QBuffer buffer;
	QDataStream in(&buffer);

	QByteArray ba = soc->readAll();
	soc->deleteLater();

	buffer.setData(ba);
	buffer.open(QBuffer::ReadOnly);

	QVariant v;
	in >> v;

#ifdef DEBUG_OUTPUT
	qDebug() << "QompInstanceWatcher::dataReady()" << v;
#endif
	QJsonDocument doc = QJsonDocument::fromVariant(v);

	if(!doc.isNull() && doc.isObject()) {
		QJsonObject obj = doc.object();
		if(obj.contains(COMMAND)) {
			const QString com = obj.value(COMMAND).toString();
			if(com == COM_SHOW) {
				emit commandShow();
			}
			else if(com == COM_TUNE) {
				const QString url = obj.value(VAL_URL).toString();
				emit commandTune(url);
			}
		}
	}
}

void QompInstanceWatcher::sendCommand(const QJsonDocument &doc)
{
	QBuffer buffer;
	buffer.open(QBuffer::ReadWrite);
	QDataStream out(&buffer);
	QVariant v = doc.toVariant();

#ifdef DEBUG_OUTPUT
	qDebug() << "QompInstanceWatcher::sendCommand" << v;
#endif
	out << v;

	QLocalSocket* soc = new QLocalSocket;
	connect(soc, &QLocalSocket::disconnected, soc, &QLocalSocket::deleteLater);

	soc->connectToServer(SYS_KEY);
	if(soc->waitForConnected(CONNECTION_INTERVAL)) {
		soc->write(buffer.data().constData(), buffer.size());
		soc->flush();
		soc->disconnectFromServer();
	}
	else {
		soc->deleteLater();
	}
}

bool QompInstanceWatcher::checkServerExists() const
{
	QLocalSocket soc;
	soc.connectToServer(SYS_KEY);
	return soc.waitForConnected(CONNECTION_INTERVAL);
}

void QompInstanceWatcher::setupServer()
{
	_serv.listen(SYS_KEY);
}
