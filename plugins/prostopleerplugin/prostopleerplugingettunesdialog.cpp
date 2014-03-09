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

#include "prostopleerplugingettunesdialog.h"
#include "prostopleerplugindefines.h"


#include "ui_prostopleerpluginresultswidget.h"

#include <QKeyEvent>

ProstoPleerPluginGetTunesDialog::ProstoPleerPluginGetTunesDialog(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	ui(new Ui::ProstoPleerPluginResultsWidget)
{
	setWindowTitle(PROSTOPLEER_PLUGIN_NAME);

	QWidget* widget = new QWidget(this);
	ui->setupUi(widget);
	setResultsWidget(widget);

	ui->tb_prev->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
	ui->tb_next->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));

	ui->tb_prev->hide();
	ui->lb_pages->hide();
	ui->lb_total->hide();

	connect(ui->tv_results, SIGNAL(clicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	connect(ui->tb_next, SIGNAL(clicked()), SIGNAL(next()));
	connect(ui->tb_prev, SIGNAL(clicked()), SIGNAL(prev()));
}

ProstoPleerPluginGetTunesDialog::~ProstoPleerPluginGetTunesDialog()
{
	delete ui;
}

void ProstoPleerPluginGetTunesDialog::setAuthStatus(const QString &status)
{
	ui->lb_auth->setText(status);
}

int ProstoPleerPluginGetTunesDialog::page() const
{
	return ui->lb_current->text().toInt();
}

void ProstoPleerPluginGetTunesDialog::setPage(int p)
{
	ui->lb_current->setText(QString::number(p));
}

int ProstoPleerPluginGetTunesDialog::totalPages() const
{
	return ui->lb_pages->text().toInt();
}

void ProstoPleerPluginGetTunesDialog::setTotalPages(int p)
{
	ui->lb_pages->setText(QString::number(p));
}

void ProstoPleerPluginGetTunesDialog::enableNext(bool enabled)
{
	ui->tb_next->setEnabled(enabled);
}

void ProstoPleerPluginGetTunesDialog::enablePrev(bool enabled)
{
	ui->tb_prev->setEnabled(enabled);
}

void ProstoPleerPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	ui->tv_results->setModel(model);
}
