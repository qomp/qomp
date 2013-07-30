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

#ifndef QOMPPLUGIN_H
#define QOMPPLUGIN_H

#include <QObject>

class QompOptionsPage;

class QompPlugin
{
public:
	virtual ~QompPlugin() {}

	virtual QString name() const = 0;
	virtual QString version() const = 0;
	virtual QString description() const = 0;
	virtual QompOptionsPage* options() = 0;
	virtual void setEnabled(bool enabled) = 0;
	virtual void unload() = 0;
};

Q_DECLARE_INTERFACE(QompPlugin, "Qomp.QompPlugin/0.1")

#endif // QOMPPLUGIN_H
