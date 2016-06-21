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
#include "pluginhost.h"
#include "defines.h"
#include "tune.h"

#include <QCoreApplication>
#include <QPluginLoader>
#include <QtPlugin>
#include <QDir>

#ifdef Q_OS_UNIX
#  ifndef QOMP_PLUGSDIR
#    if defined( __x86_64__) || defined(__amd64__)
#      define QOMP_PLUGSDIR "/usr/local/lib64/qomp/plugins"
#    else
#      define QOMP_PLUGSDIR "/usr/local/lib/qomp/plugins"
#    endif
#  endif
#endif

static const QString PLUGINS_OPTIONS_PREFIX = "plugins.is-enabled.";


PluginManager::PluginManager() :
	QObject(QCoreApplication::instance())
{
	loadStaticPlugins();
	loadPlugins();
	sortPlugins();
}

PluginManager::~PluginManager()
{
	foreach(const PluginPair& pp, plugins_) {
		delete pp.first;
	}
	plugins_.clear();
}

void PluginManager::loadStaticPlugins()
{
//	foreach(QObject* plugin, QPluginLoader::staticInstances()) {
//		QompPlugin* qp = qobject_cast<QompPlugin*>(plugin);
//		if(qp) {
//			bool en = isPluginEnabled(qp->name());
//			PluginPair pp = qMakePair(qp, en);
//			plugins_.append(pp);
//			qp->setEnabled(en);
//		}
//	}
}

void PluginManager::loadPlugins()
{
	foreach(const QString& d, pluginsDirs()) {
		QDir dir(d);
		if(dir.exists()) {
			foreach(const QString& file, dir.entryList(QDir::Files)) {
				PluginHost* host = new PluginHost(d + '/' + file, this);
				if(host->isValid()) {
					bool en = isPluginEnabled(host->name());
					PluginPair pp = qMakePair(host, en);
					plugins_.append(pp);
					if(en)
						en = host->load();
					emit pluginStatusChanged(host->name(), en);
				}
				else
					delete host;
			}
		}
	}
}

void PluginManager::sortPlugins()
{
	QStringList order = Options::instance()->getOption(OPTION_PLUGINS_ORDER, QStringList()).toStringList();
	for(int i = 0; i < order.count() && i < plugins_.count(); ++i) {
		const QString& name = order.at(i);
		for(int j = 0; j < plugins_.count(); ++j) {
			if(plugins_.at(j).first->name() == name) {
				plugins_.move(j, i);
				break;
			}
		}
	}
}

PluginHost *PluginManager::pluginForName(const QString &pluginName) const
{
	foreach(const PluginPair& pp, plugins_) {
		if(pp.first->name() == pluginName)
			return pp.first;
	}
	return nullptr;
}

QStringList PluginManager::pluginsDirs()
{
	QStringList dirs;
	dirs << qApp->applicationDirPath()+"/plugins";
#ifdef Q_OS_ANDROID
	dirs << "assets:/plugins";
	dirs << qApp->applicationDirPath();
#elif defined (Q_OS_MAC)
	dirs << QString ("%1/../Resources/plugins").arg(qApp->applicationDirPath());
#elif defined (Q_OS_UNIX)
	dirs << QString("%1").arg(QOMP_PLUGSDIR);
#endif
	return dirs;
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

QompOptionsPage *PluginManager::getOptions(const QString &pluginName)
{
	PluginHost* h = pluginForName(pluginName);
	if(h) {
		QompPlugin *p = h->plugin();
		if(p) {
			return p->options();
		}
	}
	return nullptr;
}

QString PluginManager::getVersion(const QString &pluginName) const
{
	PluginHost* h = pluginForName(pluginName);
	if(h) {
		return h->version();
	}
	return QString();
}

QString PluginManager::getDescription(const QString &pluginName) const
{
	PluginHost *p = pluginForName(pluginName);
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
			if(pp.second != enabled) {
				pp.second = enabled;
				if(pp.second)
					pp.first->load();
				else
					pp.first->unload();

				emit pluginStatusChanged(pluginName, enabled);
			}
		}
	}
}

QStringList PluginManager::tunePlugins() const
{
	QStringList list;
	foreach(const PluginPair& pp, plugins_) {
		if(!pp.second)
			continue;
		QompTunePlugin *p = qobject_cast<QompTunePlugin*>(pp.first->instance());
		if(p)
			list.append(pp.first->name());
	}

	return list;
}

QList<QompPluginAction *> PluginManager::tunesActions()
{
	QList<QompPluginAction *> l;
	foreach(const PluginPair& pp, plugins_) {
		if(!pp.second)
			continue;
		QompTunePlugin *p = qobject_cast<QompTunePlugin*>(pp.first->instance());
		if(p)
			l.append(p->getTunesActions());
	}
	return l;
}

TuneURLResolveStrategy *PluginManager::urlResolveStrategy(const QString &name) const
{
	foreach(const PluginPair& pp, plugins_) {
		if(!pp.second)
			continue;
		QompTunePlugin *p = qobject_cast<QompTunePlugin*>(pp.first->instance());
		if(p) {
			TuneURLResolveStrategy *rs = p->urlResolveStrategy();
			if(rs && rs->name() == name)
				return rs;
		}
	}
	return nullptr;
}

bool PluginManager::processUrl(const QString &url, QList<Tune *> *tunes)
{
	bool res = false;
	foreach(const PluginPair& pp, plugins_) {
		if(!pp.second)
			continue;

		QompTunePlugin *p = qobject_cast<QompTunePlugin*>(pp.first->instance());
		if(p) {
			res = p->processUrl(url, tunes);
			if(res)
				break;
		}
	}
	return res;
}

void PluginManager::qompPlayerChanged(QompPlayer *player)
{
	foreach(const PluginPair& pp, plugins_) {
		if(!pp.second)
			continue;
		QompPlayerStatusPlugin *p = qobject_cast<QompPlayerStatusPlugin*>(pp.first->instance());
		if(p) {
			p->qompPlayerChanged(player);
		}
	}
}


PluginManager* PluginManager::instance_ = nullptr;
