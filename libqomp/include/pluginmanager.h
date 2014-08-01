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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QStringList>
#include <QPair>
#include "libqomp_global.h"


class PluginHost;
class QompOptionsPage;
class TuneURLResolveStrategy;
class QompPlayer;
class QompPluginAction;


class LIBQOMPSHARED_EXPORT PluginManager : public QObject
{
	Q_OBJECT
public:
	static PluginManager* instance();
	virtual ~PluginManager();
	QStringList availablePlugins() const;
	QompOptionsPage* getOptions(const QString& pluginName);
	QString getVersion(const QString& pluginName) const;
	QString getDescription(const QString& pluginName) const;
	bool isPluginEnabled(const QString& pluginName) const;
	void setPluginEnabled(const QString& pluginName, bool enabled);
	QStringList tunePlugins() const;
	QList<QompPluginAction*> tunesActions();
	TuneURLResolveStrategy* urlResolveStrategy(const QString& strategyName) const;
	void qompPlayerChanged(QompPlayer* player);
	void sortPlugins();

signals:
	void pluginStatusChanged(const QString& pluginName, bool loaded);
	
private:
	PluginManager();
	void loadStaticPlugins();
	void loadPlugins();
	PluginHost* pluginForName(const QString& pluginName) const;
	static QStringList pluginsDirs();

private:
	static PluginManager* instance_;
	typedef QPair<PluginHost*, bool> PluginPair;
	QList<PluginPair> plugins_;
};

#endif // PLUGINMANAGER_H
