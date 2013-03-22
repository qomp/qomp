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

#include "prostopleerpluginsettings.h"
#include "ui_prostopleerpluginsettings.h"

#include "options.h"
#include "prostopleerplugindefines.h"

ProstopleerPluginSettings::ProstopleerPluginSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProstopleerPluginSettings)
{
	ui->setupUi(this);
	restore();
}

ProstopleerPluginSettings::~ProstopleerPluginSettings()
{
	delete ui;
}

void ProstopleerPluginSettings::apply()
{
	Options::instance()->setOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN, ui->le_login->text());
	Options::instance()->setOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD, ui->le_pass->text());
}


void ProstopleerPluginSettings::restore()
{
	ui->le_login->setText(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN).toString());
	ui->le_pass->setText(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD).toString());
}
