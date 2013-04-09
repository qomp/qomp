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

#include "qompoptionsmain.h"
#include "defines.h"
#include "options.h"
#include "common.h"
#include "ui_qompoptionsmain.h"

#include <phonon/BackendCapabilities>

QompOptionsMain::QompOptionsMain(QWidget *parent) :
	QompOptionsPage(parent),
	ui(new Ui::QompOptionsMain)
{
	ui->setupUi(this);
	restoreOptions();
}

QompOptionsMain::~QompOptionsMain()
{
	delete ui;
}

void QompOptionsMain::applyOptions()
{
	Options::instance()->setOption(OPTION_AUTOSTART_PLAYBACK, ui->cb_autostartPlayback->isChecked());
	Options::instance()->setOption(OPTION_START_MINIMIZED, ui->cb_minimized->isChecked());
	Options::instance()->setOption(OPTION_PROXY_HOST, ui->le_host->text());
	Options::instance()->setOption(OPTION_PROXY_PASS, encodePassword(ui->le_pass->text(), DECODE_KEY));
	Options::instance()->setOption(OPTION_PROXY_PORT, ui->le_port->text());
	Options::instance()->setOption(OPTION_PROXY_USER, ui->le_user->text());
	Options::instance()->setOption(OPTION_PROXY_USE, ui->gb_proxy->isChecked());
	Options::instance()->setOption(OPTION_PROXY_TYPE, ui->cb_proxy_type->currentText());
	Options::instance()->setOption(OPTION_AUDIO_DEVICE, ui->cb_output->itemData(ui->cb_output->currentIndex()));
	Options::instance()->setOption(OPTION_UPDATE_METADATA, ui->cb_metaData->isChecked());
}

void QompOptionsMain::restoreOptions()
{
	ui->cb_autostartPlayback->setChecked(Options::instance()->getOption(OPTION_AUTOSTART_PLAYBACK).toBool());
	ui->cb_minimized->setChecked(Options::instance()->getOption(OPTION_START_MINIMIZED).toBool());
	ui->gb_proxy->setChecked(Options::instance()->getOption(OPTION_PROXY_USE).toBool());
	ui->le_host->setText(Options::instance()->getOption(OPTION_PROXY_HOST).toString());
	ui->le_pass->setText(decodePassword(Options::instance()->getOption(OPTION_PROXY_PASS).toString(), DECODE_KEY));
	ui->le_port->setText(Options::instance()->getOption(OPTION_PROXY_PORT,"3128").toString());
	ui->le_user->setText(Options::instance()->getOption(OPTION_PROXY_USER).toString());
	ui->cb_proxy_type->setCurrentIndex(ui->cb_proxy_type->findText(Options::instance()->getOption(OPTION_PROXY_TYPE, "HTTP").toString()));
	ui->cb_metaData->setChecked(Options::instance()->getOption(OPTION_UPDATE_METADATA, false).toBool());

	ui->cb_output->addItem(tr("default"), -1);
	QList<Phonon::AudioOutputDevice> audioOutputDevices = Phonon::BackendCapabilities::availableAudioOutputDevices();
	foreach(Phonon::AudioOutputDevice dev, audioOutputDevices) {
		ui->cb_output->addItem(dev.name(), dev.index());
	}
	int index = Options::instance()->getOption(OPTION_AUDIO_DEVICE, -1).toInt();
	ui->cb_output->setCurrentIndex(ui->cb_output->findData(index));
}
