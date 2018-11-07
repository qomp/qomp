/*
 * Copyright (C) 2018  Khryukin Evgeny
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

#include "poiskmplugingettunesdialog.h"
#include "poiskmplugindefines.h"

#include "ui_poiskmpluginresultswidget.h"

#include <QKeyEvent>

class PoiskmPluginGetTunesDialog::Private
{
public:
	Private() : ui(new Ui::PoiskmPluginResultsWidget) {}
	Ui::PoiskmPluginResultsWidget *ui;
};


PoiskmPluginGetTunesDialog::PoiskmPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	p = new Private;

	setWindowTitle(POISKM_PLUGIN_NAME);

	QWidget* widget = new QWidget();
	p->ui->setupUi(widget);
	setResultsWidget(widget);

	p->ui->tb_prev->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowLeft));
	p->ui->tb_next->setIcon(qApp->style()->standardIcon(QStyle::SP_ArrowRight));

	p->ui->tb_prev->hide();
	p->ui->lb_pages->hide();
	p->ui->lb_total->hide();

	connect(p->ui->tv_results, SIGNAL(itemActivated(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	connect(p->ui->tb_next, SIGNAL(clicked()), SIGNAL(next()));
	connect(p->ui->tb_prev, SIGNAL(clicked()), SIGNAL(prev()));
}

PoiskmPluginGetTunesDialog::~PoiskmPluginGetTunesDialog()
{
	delete p->ui;
	delete p;
}

int PoiskmPluginGetTunesDialog::page() const
{
	return p->ui->lb_current->text().toInt();
}

void PoiskmPluginGetTunesDialog::setPage(int page)
{
	p->ui->lb_current->setText(QString::number(page));
}

int PoiskmPluginGetTunesDialog::totalPages() const
{
	return p->ui->lb_pages->text().toInt();
}

void PoiskmPluginGetTunesDialog::setTotalPages(int pages)
{
	p->ui->lb_pages->setText(QString::number(pages));
}

void PoiskmPluginGetTunesDialog::enableNext(bool enabled)
{
	p->ui->tb_next->setEnabled(enabled);
}

void PoiskmPluginGetTunesDialog::enablePrev(bool enabled)
{
	p->ui->tb_prev->setEnabled(enabled);
}

void PoiskmPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	p->ui->tv_results->setModel(model);
}
