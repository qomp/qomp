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

#include "qompplugingettunesdlg.h"
#include "qompnetworkingfactory.h"
#include "defines.h"
#include "options.h"

#include "ui_qompplugingettunesdlg.h"

#include <QKeyEvent>

QompPluginGettunesDlg::QompPluginGettunesDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QompPluginGettunesDlg)
{
	ui->setupUi(this);
	nam_ = QompNetworkingFactory::instance()->getNetworkAccessManager();

	QStringList searchHistory = Options::instance()->getOption(OPTION_SEARCH_HISTORY).toStringList();

	ui->cb_search->addItems(searchHistory);
	connect(ui->pb_search, SIGNAL(clicked()), SLOT(doSearch()));
}

QompPluginGettunesDlg::~QompPluginGettunesDlg()
{
	QStringList searchHistory;
	for(int i = 0; (i < ui->cb_search->count() && i < 10); i++) {
		searchHistory.append(ui->cb_search->itemText(i));
	}
	Options::instance()->setOption(OPTION_SEARCH_HISTORY, searchHistory);
	delete ui;
}

void QompPluginGettunesDlg::setResultsWidget(QWidget *widget)
{
	ui->mainLayout->insertWidget(1, widget);
}

TuneList QompPluginGettunesDlg::getTunes() const
{
	return tunes_;
}

QString QompPluginGettunesDlg::currentSearchText() const
{
	return ui->cb_search->currentText();
}

void QompPluginGettunesDlg::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Return && ui->cb_search->hasFocus()) {
		doSearch();
		e->accept();
		return;
	}
	return QDialog::keyPressEvent(e);
}
