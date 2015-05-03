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

#include "lastfmsettings.h"
#include "options.h"
#include "common.h"
#include "lastfmdefines.h"

#ifndef Q_OS_ANDROID
#include "ui_lastfmsettings.h"

class LastFmSettings::Private
{
public:
	explicit Private(LastFmSettings* p) :
		page_(p),
		widget_(new QWidget),
		ui(new Ui::LastFmSettings)
	{
		ui->setupUi(widget_);
		QObject::connect(ui->pb_authentication, SIGNAL(clicked()), page_, SIGNAL(doLogin()));
	}

	LastFmSettings* page_;
	QWidget* widget_;
	Ui::LastFmSettings* ui;
};
#else
#include "qompqmlengine.h"
#include <QQuickItem>

class LastFmSettings::Private
{
public:
	explicit Private(LastFmSettings* p) :
		page_(p)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/LastfmOptionsPage.qml"));
		QObject::connect(item_, SIGNAL(clicked()), page_, SIGNAL(doLogin()));
	}

	LastFmSettings* page_;
	QQuickItem* item_;
};
#endif

LastFmSettings::LastFmSettings(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	restoreOptions();
}

LastFmSettings::~LastFmSettings()
{
#ifndef Q_OS_ANDROID
	delete d->ui;
#endif
	delete d;
}

void LastFmSettings::retranslate()
{
#ifndef Q_OS_ANDROID
	d->ui->retranslateUi(d->widget_);
#endif
}

QObject *LastFmSettings::page() const
{
#ifndef Q_OS_ANDROID
	return d->widget_;
#else
	return d->item_;
#endif
}

void LastFmSettings::applyOptions()
{
}

void LastFmSettings::restoreOptions()
{
#ifndef Q_OS_ANDROID
	d->ui->lb_username->setText(Options::instance()->getOption(LASTFM_OPT_USER).toString());
#else
	d->item_->setProperty("login", Options::instance()->getOption(LASTFM_OPT_USER));
#endif
}
