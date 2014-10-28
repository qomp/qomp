/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#include "qompoptionsmain.h"
#include "defines.h"
#include "options.h"
#include "common.h"
#include "qompplayer.h"
#include "qompqmlengine.h"
#include "translator.h"

#include <QQuickItem>

static const QString defaultDevice = QObject::tr("default");

class QompOptionsMain::Private
{
public:
	Private(QompOptionsMain* page) :
		page_(page),
		item_(0)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/OptionsPageMain.qml"));
	}

	void applyOptions();
	void restoreOptions();

	QompOptionsMain* page_;
	QQuickItem* item_;
};


void QompOptionsMain::Private::applyOptions()
{
	Options* o = Options::instance();

	o->setOption(OPTION_AUTOSTART_PLAYBACK, item_->property("autoStartPlay"));
	o->setOption(OPTION_DEFAULT_ENCODING, item_->property("metadataEncoding"));
	o->setOption(OPTION_CURRENT_TRANSLATION, item_->property("curLang"));
//	o->setOption(OPTION_THEME, ui->cb_theme->currentText());
}

void QompOptionsMain::Private::restoreOptions()
{
	Options* o = Options::instance();
	item_->setProperty("autoStartPlay", o->getOption(OPTION_AUTOSTART_PLAYBACK).toBool());
	item_->setProperty("metadataEncoding" ,o->getOption(OPTION_DEFAULT_ENCODING).toByteArray());
	item_->setProperty("langs", Translator::instance()->availableTranslations());
	item_->setProperty("curLang", o->getOption(OPTION_CURRENT_TRANSLATION));

//	ui->cb_theme->clear();
//	ui->cb_theme->addItems(ThemeManager::instance()->availableThemes());
//	const QString them = o->getOption(OPTION_THEME).toString();
//	int i = ui->cb_theme->findText(them);
//	ui->cb_theme->setCurrentIndex(i);
}

QompOptionsMain::QompOptionsMain(QObject *parent) :
	QompOptionsPage(parent),
	player_(0)
{
	d = new Private(this);
	restoreOptions();
}

QompOptionsMain::~QompOptionsMain()
{
	//delete d->item_;
	delete d;
}

void QompOptionsMain::retranslate()
{
	//d->ui->retranslateUi(d->widget_);
}

void QompOptionsMain::init(QompPlayer *player)
{
	player_ = player;
}

QObject *QompOptionsMain::page() const
{
	return d->item_;
}

void QompOptionsMain::applyOptions()
{
	d->applyOptions();
}

void QompOptionsMain::restoreOptions()
{
	d->restoreOptions();
}

