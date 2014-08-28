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
#include "ui_lastfmsettings.h"
#include "options.h"
#include "common.h"
#include "lastfmdefines.h"


class LastFmSettings::Private
{
public:
	Private(LastFmSettings* p) :
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

LastFmSettings::LastFmSettings(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	restoreOptions();
}

LastFmSettings::~LastFmSettings()
{
	delete d->ui;
	delete d;
}

void LastFmSettings::retranslate()
{
	d->ui->retranslateUi(d->widget_);
}

QObject *LastFmSettings::page() const
{
	return d->widget_;
}

void LastFmSettings::applyOptions()
{
}

void LastFmSettings::restoreOptions()
{
	d->ui->lb_username->setText(Options::instance()->getOption(LASTFM_OPT_USER).toString());
}
