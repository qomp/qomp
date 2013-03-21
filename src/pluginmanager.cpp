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

Q_IMPORT_PLUGIN(prostopleerplugin)
Q_IMPORT_PLUGIN(filesystemplugin)
Q_IMPORT_PLUGIN(urlplugin)

PluginManager* PluginManager::instance_ = 0;
