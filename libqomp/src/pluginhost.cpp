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

#include "pluginhost.h"
#include "qompplugin.h"

#include <QPluginLoader>
#include <QFileInfo>

#ifdef HAVE_QT5
#include <QJsonObject>
#endif

#ifdef DEBUG_OUTPUT
#include <QtDebug>
#endif

PluginHost::PluginHost(const QString &file, QObject *parent) :
	QObject(parent),
	fileName_(file),
	loader_(new QPluginLoader(file, this)),
	valid_(false)
{
#ifdef HAVE_QT5
	QJsonObject obj = loader_->metaData();
	if(!obj.isEmpty() && obj.value("IID").toString().startsWith("Qomp.QompPlugin") ) {
		valid_ = true;
		QJsonObject md = obj.value("MetaData").toObject();
		if(!md.isEmpty()) {
			version_ = md.value("version").toString();
			name_ = md.value("name").toString();
			description_ = md.value("description").toString();
		}
	}
#endif
}

PluginHost::~PluginHost()
{
	unload();
	delete loader_;
	loader_ = 0;
}

bool PluginHost::load()
{
	if(!loader_)
		loader_ = new QPluginLoader(fileName_, this);

	if(loader_->isLoaded())
		return true;

	QompPlugin* p = qobject_cast<QompPlugin*>(loader_->instance());
	if(p) {
		p->setEnabled(true);
		version_ = p->version();
		description_ = p->description();
		name_ = p->name();
		return true;
	}

	loader_->unload();
	delete loader_;
	loader_ = 0;

	return false;
}

void PluginHost::unload()
{
	if(!loader_)
		return;

	QompPlugin* p = plugin();
	if(p) {
		p->setEnabled(false);
		p->unload();
	}
	loader_->unload();
	delete loader_;
	loader_ = 0;
}

bool PluginHost::isValid()
{
#ifdef HAVE_QT5
	return valid_;
#else
	if(!load())
		return false;

	unload();
#endif
	return true;
}

QString PluginHost::name() const
{
	return name_;
}

QString PluginHost::version() const
{
	return version_;
}

QString PluginHost::description() const
{
	return description_;
}

QObject *PluginHost::instance() const
{
	if(loader_ && loader_->isLoaded())
		return loader_->instance();

	return 0;
}

QompPlugin *PluginHost::plugin() const
{
	return qobject_cast<QompPlugin*>(instance());
}
