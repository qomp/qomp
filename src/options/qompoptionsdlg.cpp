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


#include "pluginmanager.h"
#include "qompoptionsmain.h"
#include "options.h"
#include "ui_qompoptionsdlg.h"

#include <QAbstractButton>
#include <QListWidgetItem>

QompOptionsDlg::QompOptionsDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QompOptionsDlg)
{
	ui->setupUi(this);
	QList<QompOptionsPage*> list;
	list << new QompOptionsMain;

	foreach(QompOptionsPage* page, list) {
		ui->sw_pages->addWidget(page);
		QListWidgetItem* it = new QListWidgetItem(ui->lw_pagesNames);
		it->setText(page->name());
		ui->lw_pagesNames->addItem(it);
	}

	foreach(QString p, PluginManager::instance()->availablePlugins()) {
		QWidget *w = PluginManager::instance()->getOptions(p);
		if(w) {
			ui->sw_pages->addWidget(w);
			QListWidgetItem* it = new QListWidgetItem(ui->lw_pagesNames);
			it->setText(p);
			ui->lw_pagesNames->addItem(it);
		}
	}
	ui->sw_pages->setCurrentIndex(0);
	ui->lw_pagesNames->setCurrentRow(0);

	connect(ui->lw_pagesNames, SIGNAL(currentRowChanged(int)), SLOT(itemChanged(int)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(buttonClicked(QAbstractButton*)));

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
}

void QompOptionsDlg::buttonClicked(QAbstractButton *b)
{
	if(ui->buttonBox->standardButton(b) == QDialogButtonBox::Apply) {
		applyOptions();
	}
}
