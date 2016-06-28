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
#include <QTimer>
#include <QBuffer>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonDocument>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

static const QString SYS_KEY = "qomp-instance";
static int MAX_SIZE = 2048;
static int TIMER_INTERVAL = 100;
static const QString COMMAND = "command";
static const QString ARG = "arg";
static const QString COM_SHOW = "show";
static const QString COM_TUNE = "tune";
static const QString VAL_URL  = "url";

QompInstanceWatcher::QompInstanceWatcher(QObject *parent) :
	QObject(parent),
	_mem(SYS_KEY),
	_firstInstance(true),
	_timer(new QTimer(this))
{
	_firstInstance = _mem.create(MAX_SIZE);

	_timer->setInterval(TIMER_INTERVAL);
	connect(_timer, &QTimer::timeout, this, &QompInstanceWatcher::pull);
	_timer->start();
}

QompInstanceWatcher::~QompInstanceWatcher()
{
	if(_mem.isAttached())
		_mem.detach();
}

bool QompInstanceWatcher::newInstanceAllowed() const
{
	return _firstInstance || !Options::instance()->getOption(OPTION_ONE_COPY).toBool();
}

int QompInstanceWatcher::pullInterval()
{
	return TIMER_INTERVAL;
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

void QompInstanceWatcher::pull()
{
	if(!_mem.isAttached())
		return;

	QBuffer buffer;
	QDataStream in(&buffer);

	_mem.lock();
	buffer.setData((char*)_mem.constData(), _mem.size());
	buffer.open(QBuffer::ReadWrite);
	QVariant v;
	in >> v;
	memset(_mem.data(), 0, _mem.size());
	_mem.unlock();

#ifdef DEBUG_OUTPUT
	qDebug() << v;
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
	qDebug() << v;
#endif
	out << v;
	int size = buffer.size();

	if (!_mem.isAttached() && !_mem.attach()) {
		return;
	}

	_mem.lock();
	char *to = (char*)_mem.data();
	const char *from = buffer.data().data();
	memcpy(to, from, qMin(_mem.size(), size));
	_mem.unlock();
}
