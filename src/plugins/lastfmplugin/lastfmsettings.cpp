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


LastFmSettings::LastFmSettings(QWidget *parent) :
	QompOptionsPage(parent),
	ui(new Ui::LastFmSettings)
{
	ui->setupUi(this);
	restoreOptions();
	connect(ui->pb_authentication, SIGNAL(clicked()), SIGNAL(optionsChanged()));
}

LastFmSettings::~LastFmSettings()
{
	delete ui;
}

void LastFmSettings::retranslate()
{
	ui->retranslateUi(this);
}

void LastFmSettings::applyOptions()
{
	Options::instance()->setOption(LASTFM_OPT_ENABLED, ui->cb_enable->isChecked());
}

void LastFmSettings::restoreOptions()
{
	ui->cb_enable->setChecked(Options::instance()->getOption(LASTFM_OPT_ENABLED).toBool());
	ui->lb_username->setText(Options::instance()->getOption(LASTFM_OPT_USER).toString());
	ui->pb_authentication->setEnabled(ui->cb_enable->isChecked());
}
