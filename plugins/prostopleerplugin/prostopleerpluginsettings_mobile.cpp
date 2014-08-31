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

#include "prostopleerpluginsettings.h"

#include "options.h"
#include "common.h"
#include "prostopleerplugindefines.h"
#include "qompqmlengine.h"

#include <QQuickItem>

class ProstopleerPluginSettings::Private
{
public:
	Private(ProstopleerPluginSettings* p) :
		page_(p),
		item_(0)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/ProstoPleerOptions.qml"));
	}

	ProstopleerPluginSettings* page_;
	QQuickItem* item_;
};

ProstopleerPluginSettings::ProstopleerPluginSettings(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	restoreOptions();
}

ProstopleerPluginSettings::~ProstopleerPluginSettings()
{
	//delete d->item_;
	delete d;
}

QString ProstopleerPluginSettings::name() const
{
	return PROSTOPLEER_PLUGIN_NAME;
}

void ProstopleerPluginSettings::retranslate()
{
	//d->ui->retranslateUi(d->widget_);
}

QObject *ProstopleerPluginSettings::page() const
{
	return d->item_;
}

void ProstopleerPluginSettings::applyOptions()
{
	Options::instance()->setOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN, d->item_->property("login"));
	Options::instance()->setOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD, Qomp::encodePassword(d->item_->property("password").toString(), PROSTOPLEER_DECODE_KEY));
}

void ProstopleerPluginSettings::restoreOptions()
{
	d->item_->setProperty("login",Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN));
	d->item_->setProperty("password",Qomp::decodePassword(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD).toString(), PROSTOPLEER_DECODE_KEY));
}

