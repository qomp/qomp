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

#include "qompoptionsplugins.h"
#include "pluginmanager.h"

#include "ui_qompoptionsplugins.h"

QompOptionsPlugins::QompOptionsPlugins(QWidget *parent) :
	QompOptionsPage(parent),
	ui(new Ui::QompOptionsPlugins)
{
	ui->setupUi(this);
	restoreOptions();
}

QompOptionsPlugins::~QompOptionsPlugins()
{
	delete ui;
}

void QompOptionsPlugins::applyOptions()
{
	for(int i = 0; i < ui->tw_plugins->topLevelItemCount(); i++) {
		QTreeWidgetItem* it = ui->tw_plugins->topLevelItem(i);
		PluginManager::instance()->setPluginEnabled(it->text(0), it->data(0, Qt::CheckStateRole).toBool());
	}
}

void QompOptionsPlugins::restoreOptions()
{
	ui->tw_plugins->clear();
	foreach(const QString& name, PluginManager::instance()->availablePlugins()) {
		QTreeWidgetItem* it = new QTreeWidgetItem(ui->tw_plugins);
		it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
		it->setData(0, Qt::CheckStateRole, PluginManager::instance()->isPluginEnabled(name) ? 2 : 0);
		it->setText(0, name);
		it->setText(1, PluginManager::instance()->getVersion(name));
		it->setText(2, PluginManager::instance()->getDescription(name));
		ui->tw_plugins->addTopLevelItem(it);
	}
	ui->tw_plugins->resizeColumnToContents(0);
}
