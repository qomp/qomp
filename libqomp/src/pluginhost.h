/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#ifndef PLUGINHOST_H
#define PLUGINHOST_H

#include <QObject>

class QPluginLoader;
class QompPlugin;

class PluginHost : public QObject
{
	Q_OBJECT
public:
	PluginHost(const QString& file, QObject *parent = 0);
	~PluginHost();

	bool load();
	void unload();
	bool isValid();
	QString name() const;
	QString version() const;
	QString description() const;
	QObject* instance() const;
	QompPlugin* plugin() const;

private:
	QString fileName_;
	QPluginLoader* loader_;
	QString version_;
	QString description_;
	QString name_;
	bool valid_;
};

#endif // PLUGINHOST_H
