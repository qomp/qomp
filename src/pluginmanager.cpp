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

#include "pluginmanager.h"
#include "qompplugin.h"

#include <QCoreApplication>
#include <QPluginLoader>
#include <QtPlugin>


PluginManager::PluginManager() :
	QObject(QCoreApplication::instance())
{
	loadStaticPlugins();
	loadPlugins();
}

void PluginManager::loadStaticPlugins()
{
	foreach(QObject* plugin, QPluginLoader::staticInstances()) {
		QompPlugin* qp = qobject_cast<QompPlugin*>(plugin);
		if(qp) {
			plugins_.append(qp);
		}
	}
}

void PluginManager::loadPlugins()
{
}


PluginManager *PluginManager::instance()
{
	if(!instance_)
		instance_ = new PluginManager();

	return instance_;
}

QStringList PluginManager::availablePlugins() const
{
	QStringList list;
	foreach(QompPlugin* p, plugins_)
		list.append(p->name());

	return list;
}

TuneList PluginManager::getTune(const QString &pluginName)
{
	TuneList tl;
	foreach(QompPlugin* p, plugins_) {
		if(p->name() == pluginName)
			tl = p->getTunes();
	}

	return tl;
}

QompOptionsPage *PluginManager::getOptions(const QString &pluginName)
{
	foreach(QompPlugin* p, plugins_) {
		if(p->name() == pluginName)
			return p->options();
	}
	return 0;
}

QString PluginManager::getVersion(const QString &pluginName) const
{
	foreach(QompPlugin* p, plugins_) {
		if(p->name() == pluginName)
			return p->version();
	}
	return QString();
}

QString PluginManager::getDescription(const QString &pluginName) const
{
	foreach(QompPlugin* p, plugins_) {
		if(p->name() == pluginName)
			return p->description();
	}
	return QString();
}

Q_IMPORT_PLUGIN(myzukaruplugin)
Q_IMPORT_PLUGIN(prostopleerplugin)
Q_IMPORT_PLUGIN(filesystemplugin)
Q_IMPORT_PLUGIN(urlplugin)

PluginManager* PluginManager::instance_ = 0;
