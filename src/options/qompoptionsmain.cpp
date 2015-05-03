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
#include "translator.h"
#include "qomptrayicon.h"
#include "thememanager.h"
#include "ui_qompoptionsmain.h"

static const QString defaultDevice = QObject::tr("default");

class QompOptionsMain::Private
{
public:
	explicit Private(QompOptionsMain* page) :
		page_(page),
		ui(new Ui::QompOptionsMain),
		widget_(new QWidget)
	{
		ui->setupUi(widget_);

		ui->cb_metaData->hide();
//		ui->label_8->hide();
//		ui->le_encoding->hide();
	}

	void applyOptions();
	void restoreOptions();

	QompOptionsMain* page_;
	Ui::QompOptionsMain *ui;
	QWidget* widget_;
};


void QompOptionsMain::Private::applyOptions()
{
	Options* o = Options::instance();

	o->setOption(OPTION_AUTOSTART_PLAYBACK, ui->cb_autostartPlayback->isChecked());
	o->setOption(OPTION_START_MINIMIZED, ui->cb_minimized->isChecked());
	o->setOption(OPTION_PROXY_HOST, ui->le_host->text());
	o->setOption(OPTION_PROXY_PASS, Qomp::encodePassword(ui->le_pass->text(), DECODE_KEY));
	o->setOption(OPTION_PROXY_PORT, ui->le_port->text());
	o->setOption(OPTION_PROXY_USER, ui->le_user->text());
	o->setOption(OPTION_PROXY_USE, ui->gb_proxy->isChecked());
	o->setOption(OPTION_PROXY_TYPE, ui->cb_proxy_type->currentText());
	o->setOption(OPTION_AUDIO_DEVICE, ui->cb_output->currentText());
	o->setOption(OPTION_UPDATE_METADATA, ui->cb_metaData->isChecked());
	o->setOption(OPTION_HIDE_ON_CLOSE, ui->cb_hideOnClose->isChecked());
	o->setOption(OPTION_DEFAULT_ENCODING, ui->le_encoding->text().toUtf8());
	o->setOption(OPTION_TRAY_DOUBLE_CLICK, ui->cb_doubleClick->currentIndex());
	o->setOption(OPTION_TRAY_MIDDLE_CLICK, ui->cb_middleClick->currentIndex());
	o->setOption(OPTION_TRAY_LEFT_CLICK, ui->cb_leftClick->currentIndex());
	o->setOption(OPTION_THEME, ui->cb_theme->currentText());
	o->setOption(OPTION_CURRENT_TRANSLATION,ui->cb_lang->currentText());
}

void QompOptionsMain::Private::restoreOptions()
{
	Options* o = Options::instance();
	ui->cb_autostartPlayback->setChecked(o->getOption(OPTION_AUTOSTART_PLAYBACK).toBool());
	ui->cb_minimized->setChecked(o->getOption(OPTION_START_MINIMIZED).toBool());
	ui->gb_proxy->setChecked(o->getOption(OPTION_PROXY_USE).toBool());
	ui->le_host->setText(o->getOption(OPTION_PROXY_HOST).toString());
	ui->le_pass->setText(Qomp::decodePassword(o->getOption(OPTION_PROXY_PASS).toString(), DECODE_KEY));
	ui->le_port->setText(o->getOption(OPTION_PROXY_PORT).toString());
	ui->le_user->setText(o->getOption(OPTION_PROXY_USER).toString());
	ui->cb_proxy_type->setCurrentIndex(ui->cb_proxy_type->findText(o->getOption(OPTION_PROXY_TYPE).toString()));
	ui->cb_metaData->setChecked(o->getOption(OPTION_UPDATE_METADATA).toBool());
	ui->cb_hideOnClose->setChecked(o->getOption(OPTION_HIDE_ON_CLOSE).toBool());
	ui->le_encoding->setText(o->getOption(OPTION_DEFAULT_ENCODING).toByteArray());

	ui->cb_output->clear();
	ui->cb_output->addItem(defaultDevice);
	if(page_->player_)
		ui->cb_output->addItems(page_->player_->audioOutputDevice());
	QString dev = o->getOption(OPTION_AUDIO_DEVICE, defaultDevice).toString();
	int index = ui->cb_output->findText(dev);
	if(index == -1)
		ui->cb_output->setCurrentIndex(ui->cb_output->findText(defaultDevice));
	else
		ui->cb_output->setCurrentIndex(index);

	ui->cb_lang->clear();
	ui->cb_lang->addItems(Translator::instance()->availableTranslations());
	QString curTr = o->getOption(OPTION_CURRENT_TRANSLATION).toString();
	index = ui->cb_lang->findText(curTr);
	if(index != -1)
		ui->cb_lang->setCurrentIndex(index);
	else
		ui->cb_lang->setCurrentIndex(0);

	QStringList actions = QompTrayIcon::availableActions();
	ui->cb_middleClick->addItems(actions);
	QompTrayActionType type = o->getOption(OPTION_TRAY_MIDDLE_CLICK).toInt();
	ui->cb_middleClick->setCurrentIndex(type);
	ui->cb_leftClick->addItems(actions);
	type = o->getOption(OPTION_TRAY_LEFT_CLICK).toInt();
	ui->cb_leftClick->setCurrentIndex(type);
	ui->cb_doubleClick->addItems(actions);
	type = o->getOption(OPTION_TRAY_DOUBLE_CLICK).toInt();
	ui->cb_doubleClick->setCurrentIndex(type);

	ui->cb_theme->clear();
	ui->cb_theme->addItems(ThemeManager::instance()->availableThemes());
	const QString them = o->getOption(OPTION_THEME).toString();
	int i = ui->cb_theme->findText(them);
	ui->cb_theme->setCurrentIndex(i);
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
	delete d->ui;
	delete d;
}

void QompOptionsMain::retranslate()
{
	d->ui->retranslateUi(d->widget_);
}

void QompOptionsMain::init(QompPlayer *player)
{
	player_ = player;
}

QObject *QompOptionsMain::page() const
{
	return d->widget_;
}

void QompOptionsMain::applyOptions()
{
	d->applyOptions();
}

void QompOptionsMain::restoreOptions()
{
	d->restoreOptions();
}
