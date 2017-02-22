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
#include "singleapplication/singleapplication.h"

#include <QJsonObject>
#include <QBuffer>
#include <QDataStream>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const QString COMMAND = "command";
static const QString COM_TUNE = "tune";
static const QString VAL_URL  = "url";
static const qint16  COMMAND_TIMEOUT = 500;

QompInstanceWatcher::QompInstanceWatcher(QObject *parent) :
	QObject(parent)
{
	connect(static_cast<SingleApplication*>(qApp), &SingleApplication::instanceStarted,
		this, &QompInstanceWatcher::commandShow);
	connect(static_cast<SingleApplication*>(qApp), &SingleApplication::receivedMessage,
		this, &QompInstanceWatcher::incommingCommand);
}

QompInstanceWatcher::~QompInstanceWatcher()
{
}

bool QompInstanceWatcher::newInstanceAllowed() const
{
	return !Options::instance()->getOption(OPTION_ONE_COPY).toBool()
			|| static_cast<SingleApplication*>(qApp)->isPrimary();
}

void QompInstanceWatcher::sendCommandTune(const QString &url)
{
	QJsonObject obj {
		{COMMAND, COM_TUNE},
		{VAL_URL, url}
	};

	QBuffer buffer;
	buffer.open(QBuffer::ReadWrite);
	QDataStream out(&buffer);
	out << obj.toVariantMap();

	static_cast<SingleApplication*>(qApp)->sendMessage(buffer.data(), COMMAND_TIMEOUT);
}

void QompInstanceWatcher::incommingCommand(quint32 /*instanceId*/, const QByteArray& message)
{
	if(message.size() == 0)
		return;

	QBuffer buffer;
	QDataStream in(&buffer);

	buffer.setData(message);
	buffer.open(QBuffer::ReadOnly);

	QVariantMap m;
	in >> m;

	QJsonObject obj = QJsonObject::fromVariantMap(m);

	if(!obj.isEmpty() && obj.contains(COMMAND)) {
		const QString com = obj.value(COMMAND).toString();
		if(com == COM_TUNE) {
			const QString url = obj.value(VAL_URL).toString();
			emit commandTune(url);
		}
	}
}
