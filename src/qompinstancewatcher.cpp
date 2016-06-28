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
#include <QSharedMemory>

static const QString SYS_KEY = "qomp-instance";

QompInstanceWatcher::QompInstanceWatcher(QObject *parent) :
	QObject(parent),
	_mem(new QSharedMemory(SYS_KEY)),
	_firstInstance(true)
{
	_firstInstance = _mem->create(1);
}

QompInstanceWatcher::~QompInstanceWatcher()
{
	if(_mem->isAttached())
		_mem->detach();

	delete _mem;
}

bool QompInstanceWatcher::newInstanceAllowed() const
{
	return _firstInstance || !Options::instance()->getOption(OPTION_ONE_COPY).toBool();
}
