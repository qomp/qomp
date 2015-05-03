/*
 * Copyright (C) 2014 Khryukin Evgeny
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

#include "qompoptionsplugins.h"
#include "pluginmanager.h"
#include "options.h"
#include "defines.h"
#include "qompqmlengine.h"

#include <QModelIndex>
#include <QQuickItem>


class QompOptionsPlugins::Private: public QObject
{
	Q_OBJECT
public:
	explicit Private(QompOptionsPlugins* page) :
		QObject(page),
		page_(page)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/OptionsPagePlugins.qml"));
	}

	void applyOptions();
	void restoreOptions();

public:
	QompOptionsPlugins* page_;
	QQuickItem* item_;

};

void QompOptionsPlugins::Private::applyOptions()
{
	QVariant var;
	QMetaObject::invokeMethod(item_, "getSettings", Q_RETURN_ARG(QVariant, var));
	foreach(QVariant v, var.value<QVariantList>()) {
		QVariantMap map = v.value<QVariantMap>();
		PluginManager::instance()->setPluginEnabled(map.value("name").toString(),
							    map.value("enabled").toBool());
	}
}

void QompOptionsPlugins::Private::restoreOptions()
{
	QMetaObject::invokeMethod(item_, "clearPlugins");
	foreach(const QString& name, PluginManager::instance()->availablePlugins()) {
		QMetaObject::invokeMethod(item_, "addPlugin",
					  Q_ARG(QVariant,name),
					  Q_ARG(QVariant, PluginManager::instance()->getDescription(name)),
					  Q_ARG(QVariant, PluginManager::instance()->getVersion(name)),
					  Q_ARG(QVariant, PluginManager::instance()->isPluginEnabled(name)));
	}
}




QompOptionsPlugins::QompOptionsPlugins(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	restoreOptions();
}

QompOptionsPlugins::~QompOptionsPlugins()
{
	delete d;
}

void QompOptionsPlugins::retranslate()
{
}

QObject *QompOptionsPlugins::page() const
{
	return d->item_;
}

void QompOptionsPlugins::applyOptions()
{
	d->applyOptions();
}

void QompOptionsPlugins::restoreOptions()
{
	d->restoreOptions();
}

#include "qompoptionsplugins_mobile.moc"
