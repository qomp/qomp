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

#include "qompoptionsdlg.h"
#include "qompoptionspage.h"
#include "pluginmanager.h"
#include "qompoptionsmain.h"
#include "options.h"
#include "qompmainwin.h"
#include "qompoptionsplugins.h"

#include "ui_qompoptionsdlg.h"

#include <QAbstractButton>
#include <QListWidgetItem>
#include <QKeyEvent>

QompOptionsDlg::QompOptionsDlg(QompPlayer *player, QompMainWin *parent) :
	QDialog(parent),
	ui(new Ui::QompOptionsDlg)
{
#if defined HAVE_QT5 && defined Q_OS_ANDROID
	setWindowState(Qt::WindowMaximized);
#endif
	ui->setupUi(this);
	QList<QompOptionsPage*> list;
	QompOptionsMain* om = new QompOptionsMain(this);
	QompOptionsPlugins* op = new QompOptionsPlugins(this);
	list << om << op;

	foreach(QompOptionsPage* page, list) {
		page->init(player);
		ui->sw_pages->addWidget(page);
		QListWidgetItem* it = new QListWidgetItem(ui->lw_pagesNames);
		it->setText(page->name());
		ui->lw_pagesNames->addItem(it);
	}

	foreach(const QString& p, PluginManager::instance()->availablePlugins()) {
		addPluginPage(p);
	}
	ui->sw_pages->setCurrentIndex(0);
	ui->lw_pagesNames->setCurrentRow(0);

	connect(ui->lw_pagesNames, SIGNAL(currentRowChanged(int)), SLOT(itemChanged(int)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(buttonClicked(QAbstractButton*)));

	connect(PluginManager::instance(), SIGNAL(pluginStatusChanged(QString, bool)), SLOT(pluginLoadingStatusChanged(QString,bool)));

	adjustSize();
	ui->lw_pagesNames->setFixedWidth(ui->lw_pagesNames->width());
}

QompOptionsDlg::~QompOptionsDlg()
{
	delete ui;
}

void QompOptionsDlg::accept()
{
	applyOptions();
	QDialog::accept();
}

void QompOptionsDlg::changeEvent(QEvent *e)
{
	if(e->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
		for(int i = 0; i < ui->sw_pages->count(); i++) {
			QompOptionsPage* p = static_cast<QompOptionsPage*>(ui->sw_pages->widget(i));
			p->retranslate();
			QListWidgetItem* it = ui->lw_pagesNames->item(i);
			it->setText(p->name());
		}
	}
	QDialog::changeEvent(e);
}

void QompOptionsDlg::keyReleaseEvent(QKeyEvent *ke)
{
	QDialog::keyReleaseEvent(ke);
#if defined HAVE_QT5 && defined Q_OS_ANDROID
	if(ke->key() == Qt::Key_Back) {
		reject();
	}
#endif
}

void QompOptionsDlg::applyOptions()
{
	for(int i = 0; i < ui->sw_pages->count(); i++) {
		QompOptionsPage* p = static_cast<QompOptionsPage*>(ui->sw_pages->widget(i));
		p->applyOptions();
	}

	Options::instance()->applyOptions();
}

void QompOptionsDlg::itemChanged(int row)
{
	ui->sw_pages->setCurrentIndex(row);
	static_cast<QompOptionsPage*>(ui->sw_pages->currentWidget())->restoreOptions();
}

void QompOptionsDlg::buttonClicked(QAbstractButton *b)
{
	if(ui->buttonBox->standardButton(b) == QDialogButtonBox::Apply) {
		applyOptions();
	}
}

void QompOptionsDlg::pageDestroyed()
{
	QWidget* w = static_cast<QWidget*>(sender());
	int ind = ui->sw_pages->indexOf(w);
	ui->sw_pages->removeWidget(w);
	QListWidgetItem* it = ui->lw_pagesNames->item(ind);
	ui->lw_pagesNames->removeItemWidget(it);
	delete it;
}

void QompOptionsDlg::pluginLoadingStatusChanged(const QString &pluginName, bool status)
{
	if(status)
		addPluginPage(pluginName);
}

void QompOptionsDlg::addPluginPage(const QString &name)
{
	QWidget *w = PluginManager::instance()->getOptions(name);
	if(w) {
		ui->sw_pages->addWidget(w);
		QListWidgetItem* it = new QListWidgetItem(ui->lw_pagesNames);
		it->setText(name);
		ui->lw_pagesNames->addItem(it);
		connect(w, SIGNAL(destroyed()), SLOT(pageDestroyed()));
	}
}
