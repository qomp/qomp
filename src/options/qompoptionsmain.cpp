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
#include "qompplayer.h"
#include "translator.h"
#include "qomptrayicon.h"
#include "ui_qompoptionsmain.h"

static const QString defaultDevice = QObject::tr("default");

QompOptionsMain::QompOptionsMain(QWidget *parent) :
	QompOptionsPage(parent),
	ui(new Ui::QompOptionsMain),
	player_(QompPlayer::instance())
{
	ui->setupUi(this);
	restoreOptions();

	//Metadata resolving doesn`t work normal in qt5, so hide this option
#ifdef HAVE_QT5
	ui->cb_metaData->hide();
#endif
}

QompOptionsMain::~QompOptionsMain()
{
	delete ui;
}

void QompOptionsMain::retranslate()
{
	ui->retranslateUi(this);
}

void QompOptionsMain::applyOptions()
{
	Options::instance()->setOption(OPTION_AUTOSTART_PLAYBACK, ui->cb_autostartPlayback->isChecked());
	Options::instance()->setOption(OPTION_START_MINIMIZED, ui->cb_minimized->isChecked());
	Options::instance()->setOption(OPTION_PROXY_HOST, ui->le_host->text());
	Options::instance()->setOption(OPTION_PROXY_PASS, Qomp::encodePassword(ui->le_pass->text(), DECODE_KEY));
	Options::instance()->setOption(OPTION_PROXY_PORT, ui->le_port->text());
	Options::instance()->setOption(OPTION_PROXY_USER, ui->le_user->text());
	Options::instance()->setOption(OPTION_PROXY_USE, ui->gb_proxy->isChecked());
	Options::instance()->setOption(OPTION_PROXY_TYPE, ui->cb_proxy_type->currentText());
	Options::instance()->setOption(OPTION_AUDIO_DEVICE, ui->cb_output->currentText());
	Options::instance()->setOption(OPTION_UPDATE_METADATA, ui->cb_metaData->isChecked());
	Options::instance()->setOption(OPTION_HIDE_ON_CLOSE, ui->cb_hideOnClose->isChecked());
	Options::instance()->setOption(OPTION_DEFAULT_ENCODING, ui->le_encoding->text().toUtf8());
	Options::instance()->setOption(OPTION_TRAY_DOUBLE_CLICK, ui->cb_doubleClick->currentIndex());
	Options::instance()->setOption(OPTION_TRAY_MIDDLE_CLICK, ui->cb_middleClick->currentIndex());
	Options::instance()->setOption(OPTION_TRAY_LEFT_CLICK, ui->cb_leftClick->currentIndex());
	Translator::instance()->retranslate(ui->cb_lang->currentText());
}

void QompOptionsMain::restoreOptions()
{
	ui->cb_autostartPlayback->setChecked(Options::instance()->getOption(OPTION_AUTOSTART_PLAYBACK).toBool());
	ui->cb_minimized->setChecked(Options::instance()->getOption(OPTION_START_MINIMIZED).toBool());
	ui->gb_proxy->setChecked(Options::instance()->getOption(OPTION_PROXY_USE).toBool());
	ui->le_host->setText(Options::instance()->getOption(OPTION_PROXY_HOST).toString());
	ui->le_pass->setText(Qomp::decodePassword(Options::instance()->getOption(OPTION_PROXY_PASS).toString(), DECODE_KEY));
	ui->le_port->setText(Options::instance()->getOption(OPTION_PROXY_PORT).toString());
	ui->le_user->setText(Options::instance()->getOption(OPTION_PROXY_USER).toString());
	ui->cb_proxy_type->setCurrentIndex(ui->cb_proxy_type->findText(Options::instance()->getOption(OPTION_PROXY_TYPE).toString()));
	ui->cb_metaData->setChecked(Options::instance()->getOption(OPTION_UPDATE_METADATA).toBool());
	ui->cb_hideOnClose->setChecked(Options::instance()->getOption(OPTION_HIDE_ON_CLOSE).toBool());
	ui->le_encoding->setText(Options::instance()->getOption(OPTION_DEFAULT_ENCODING).toByteArray());

	ui->cb_output->clear();
	ui->cb_output->addItem(defaultDevice);
	if(player_)
		ui->cb_output->addItems(player_->audioOutputDevice());
	QString dev = Options::instance()->getOption(OPTION_AUDIO_DEVICE, defaultDevice).toString();
	int index = ui->cb_output->findText(dev);
	if(index == -1)
		ui->cb_output->setCurrentIndex(ui->cb_output->findText(defaultDevice));
	else
		ui->cb_output->setCurrentIndex(index);

	ui->cb_lang->clear();
	ui->cb_lang->addItems(Translator::instance()->availableTranslations());
	QString curTr = Translator::instance()->currentTranslation();
	index = ui->cb_lang->findText(curTr);
	if(index != -1)
		ui->cb_lang->setCurrentIndex(index);
	else
		ui->cb_lang->setCurrentIndex(0);

	QStringList actions = QompTrayIcon::availableActions();
	ui->cb_middleClick->addItems(actions);
	QompTrayActionType type = Options::instance()->getOption(OPTION_TRAY_MIDDLE_CLICK).toInt();
	ui->cb_middleClick->setCurrentIndex(type);
	ui->cb_leftClick->addItems(actions);
	type = Options::instance()->getOption(OPTION_TRAY_LEFT_CLICK).toInt();
	ui->cb_leftClick->setCurrentIndex(type);
	ui->cb_doubleClick->addItems(actions);
	type = Options::instance()->getOption(OPTION_TRAY_DOUBLE_CLICK).toInt();
	ui->cb_doubleClick->setCurrentIndex(type);
}
