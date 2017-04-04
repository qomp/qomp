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

class ProstoPleerPluginGetTunesDialog::Private
{
public:
	Private() : ui(new Ui::ProstoPleerPluginResultsWidget) {}
	Ui::ProstoPleerPluginResultsWidget *ui;
};


ProstoPleerPluginGetTunesDialog::ProstoPleerPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	p = new Private;

	setWindowTitle(PROSTOPLEER_PLUGIN_NAME);

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

ProstoPleerPluginGetTunesDialog::~ProstoPleerPluginGetTunesDialog()
{
	delete p->ui;
	delete p;
}

void ProstoPleerPluginGetTunesDialog::setAuthStatus(const QString &status)
{
	p->ui->lb_auth->setText(status);
}

int ProstoPleerPluginGetTunesDialog::page() const
{
	return p->ui->lb_current->text().toInt();
}

void ProstoPleerPluginGetTunesDialog::setPage(int page)
{
	p->ui->lb_current->setText(QString::number(page));
}

int ProstoPleerPluginGetTunesDialog::totalPages() const
{
	return p->ui->lb_pages->text().toInt();
}

void ProstoPleerPluginGetTunesDialog::setTotalPages(int pages)
{
	p->ui->lb_pages->setText(QString::number(pages));
}

void ProstoPleerPluginGetTunesDialog::enableNext(bool enabled)
{
	p->ui->tb_next->setEnabled(enabled);
}

void ProstoPleerPluginGetTunesDialog::enablePrev(bool enabled)
{
	p->ui->tb_prev->setEnabled(enabled);
}

void ProstoPleerPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	p->ui->tv_results->setModel(model);
}
