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
#include "options.h"
#include "qomptunepluign.h"
#include "qompplayerstatusplugin.h"

#include <QCoreApplication>
#include <QPluginLoader>
#include <QtPlugin>


static const QString PLUGINS_OPTIONS_PREFIX = "plugins.is-enabled.";

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
			bool en = isPluginEnabled(qp->name());
			PluginPair pp = qMakePair(qp, en);
			plugins_.append(pp);
		}
	}
}

void PluginManager::loadPlugins()
{
}

QompPlugin *PluginManager::pluginForName(const QString &pluginName) const
{
	foreach(const PluginPair& pp, plugins_) {
		if(pp.first->name() == pluginName)
			return pp.first;
	}
	return 0;
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
	foreach(const PluginPair& pp, plugins_)
		list.append(pp.first->name());

	return list;
}

TuneList PluginManager::getTune(const QString &pluginName)
{
	TuneList tl;
	QompTunePlugin *p = dynamic_cast<QompTunePlugin*>(pluginForName(pluginName));
	if(p) {
		tl = p->getTunes();
	}

	return tl;
}

QompOptionsPage *PluginManager::getOptions(const QString &pluginName)
{
	QompPlugin *p = pluginForName(pluginName);
	if(p) {
		return p->options();
	}
	return 0;
}

QString PluginManager::getVersion(const QString &pluginName) const
{
	QompPlugin *p = pluginForName(pluginName);
	if(p) {
		return p->version();
	}
	return QString();
}

QString PluginManager::getDescription(const QString &pluginName) const
{
	QompPlugin *p = pluginForName(pluginName);
	if(p) {
		return p->description();
	}
	return QString();
}

bool PluginManager::isPluginEnabled(const QString &pluginName) const
{
	return Options::instance()->getOption(PLUGINS_OPTIONS_PREFIX + pluginName, true).toBool();
}

void PluginManager::setPluginEnabled(const QString &pluginName, bool enabled)
{
	Options::instance()->setOption(PLUGINS_OPTIONS_PREFIX + pluginName, enabled);
	for(int i = 0; i < plugins_.size(); i++) {
		PluginPair& pp = plugins_[i];
		if(pp.first->name() == pluginName) {
			pp.second = enabled;
		}
	}
}

QStringList PluginManager::tunePlugins() const
{
	QStringList list;
	foreach(const PluginPair& pp, plugins_) {
		QompTunePlugin *p = dynamic_cast<QompTunePlugin*>(pp.first);
		if(p)
			list.append(pp.first->name());
	}

	return list;
}

TuneURLResolveStrategy *PluginManager::urlResolveStrategy(const QString &name) const
{
	foreach(const PluginPair& pp, plugins_) {
		QompTunePlugin *p = dynamic_cast<QompTunePlugin*>(pp.first);
		if(p) {
			TuneURLResolveStrategy *rs = p->urlResolveStrategy();
			if(rs && rs->name() == name)
				return rs;
		}
	}
	return 0;
}

void PluginManager::qompPlayerChanged(QompPlayer *player)
{
	foreach(const PluginPair& pp, plugins_) {
		QompPlayerStatusPlugin *p = dynamic_cast<QompPlayerStatusPlugin*>(pp.first);
		if(p) {
			p->qompPlayerChanged(player);
		}
	}
}

#ifdef HAVE_QT5
Q_IMPORT_PLUGIN(MyzukaruPlugin)
//Q_IMPORT_PLUGIN(DeezerPlugin)
Q_IMPORT_PLUGIN(YandexMusicPlugin)
Q_IMPORT_PLUGIN(ProstoPleerPlugin)
Q_IMPORT_PLUGIN(FilesystemPlugin)
Q_IMPORT_PLUGIN(UrlPlugin)
Q_IMPORT_PLUGIN(LastFmPlugin)
#else
Q_IMPORT_PLUGIN(myzukaruplugin)
//Q_IMPORT_PLUGIN(deezerplugin)
Q_IMPORT_PLUGIN(yandexmusicplugin)
Q_IMPORT_PLUGIN(prostopleerplugin)
Q_IMPORT_PLUGIN(filesystemplugin)
Q_IMPORT_PLUGIN(urlplugin)
Q_IMPORT_PLUGIN(lastfmplugin)
#endif

PluginManager* PluginManager::instance_ = 0;
