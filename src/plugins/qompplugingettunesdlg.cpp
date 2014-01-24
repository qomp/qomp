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
#include "qompplugintreemodel.h"
#include "defines.h"
#include "options.h"

#include "ui_qompplugingettunesdlg.h"

#include <QKeyEvent>
#include <QMenu>

QompPluginGettunesDlg::QompPluginGettunesDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::QompPluginGettunesDlg),
	suggestionsMenu_(new QMenu(this))
{
	ui->setupUi(this);

	QStringList searchHistory = Options::instance()->getOption(OPTION_SEARCH_HISTORY).toStringList();

	ui->cb_search->addItems(searchHistory);
	ui->cb_search->setInsertPolicy(QComboBox::InsertAtTop);
	connect(ui->pb_search, SIGNAL(clicked()), SLOT(search()));
	connect(ui->cb_search, SIGNAL(editTextChanged(QString)), SIGNAL(searchTextChanged(QString)));

	ui->lb_busy->changeText(tr("Searching"));

	ui->cb_search->installEventFilter(this);
	suggestionsMenu_->installEventFilter(this);
	connect(suggestionsMenu_, SIGNAL(triggered(QAction*)), SLOT(suggestionActionTriggered(QAction*)));
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

QompPluginGettunesDlg::Result QompPluginGettunesDlg::go()
{
	if(exec() == QDialog::Accepted)
		return ResultOK;

	return ResultCancel;
}

void QompPluginGettunesDlg::setResultsWidget(QWidget *widget)
{
	ui->mainLayout->insertWidget(1, widget);
}

void QompPluginGettunesDlg::suggestionActionTriggered(QAction *a)
{
	if(a) {
		ui->cb_search->blockSignals(true);
		const QString text = a->text();
		if(ui->cb_search->findText(text) == -1) {
			ui->cb_search->insertItem(0, text);
		}
		ui->cb_search->setCurrentIndex(ui->cb_search->findText(text));
		search();
		ui->cb_search->blockSignals(false);
	}
}

void QompPluginGettunesDlg::search()
{
	const QString text = ui->cb_search->currentText();
	if(ui->cb_search->findText(text) == -1)
		ui->cb_search->insertItem(0, text);

	emit doSearch(text);
}

QString QompPluginGettunesDlg::currentSearchText() const
{
	return ui->cb_search->currentText();
}

void QompPluginGettunesDlg::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Return && ui->cb_search->hasFocus()) {
		search();
		e->accept();
		return;
	}
	return QDialog::keyPressEvent(e);
}

bool QompPluginGettunesDlg::eventFilter(QObject *o, QEvent *e)
{
	if(o == suggestionsMenu_ && e->type() == QEvent::KeyPress) {
		QKeyEvent* ke = static_cast<QKeyEvent*>(e);
		if(ke->key() == Qt::Key_Escape) {
			suggestionsMenu_->hide();
			ui->cb_search->setFocus();
			return true;
		}
		else if(ke->key() != Qt::Key_Up
			&& ke->key() != Qt::Key_Down
			&& ke->key() != Qt::Key_Return) {
			qApp->postEvent(ui->cb_search, new QKeyEvent(ke->type(),
								     ke->key(),
								     ke->modifiers(),
								     ke->text(),
								     ke->isAutoRepeat(),
								     ushort(ke->count())));
			ke->accept();
			suggestionsMenu_->hide();
			ui->cb_search->setFocus();
			return true;
		}
	}
	//Workaround inserting combobox suggestion on menu popup
	else if(o == ui->cb_search) {
		if(/*e->type() == QEvent::FocusIn ||*/ e->type() == QEvent::FocusOut) {
			e->ignore();
			return true;
		}
	}
	return QDialog::eventFilter(o, e);
}

void QompPluginGettunesDlg::startBusyWidget()
{
	if(!ui->lb_busy->isActive())
		ui->lb_busy->start();
}

void QompPluginGettunesDlg::stopBusyWidget()
{
	ui->lb_busy->stop();
}

void QompPluginGettunesDlg::newSuggestions(const QStringList &list)
{
	suggestionsMenu_->clear();

	foreach(const QString& sug, list) {
		QAction* act = new QAction(sug, suggestionsMenu_);
		suggestionsMenu_->addAction(act);
	}
	suggestionsMenu_->popup(mapToGlobal(ui->cb_search->geometry().bottomLeft()));
	suggestionsMenu_->move(mapToGlobal(ui->cb_search->geometry().bottomLeft()));
}

void QompPluginGettunesDlg::itemSelected(const QModelIndex &ind)
{
	if(!ind.isValid())
		return;
	QAbstractItemView* view = qobject_cast<QAbstractItemView*>(sender());
	if(!view)
		return;

	QompPluginTreeModel* model = qobject_cast<QompPluginTreeModel*>(view->model());
	QompPluginModelItem* item = model->item(ind);

	emit itemSelected(item);
}
