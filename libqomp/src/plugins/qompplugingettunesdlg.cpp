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
#include "tune.h"
#include "thememanager.h"
#include "advwidget.h"

#include "ui_qompplugingettunesdlg.h"

#include <QKeyEvent>
#include <QMenu>
#include <QTimer>
#include <QDialog>
#include <QMessageBox>
#include <QEventLoop>

static const int sugTimerInterval = 500;


class QompPluginGettunesDlg::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QompPluginGettunesDlg* p = 0);
	~Private();

public slots:
	void suggestionActionTriggered(QAction* a);
	void search();
	void timeout();

protected:
	void keyPressEvent(QKeyEvent *e);
	bool eventFilter(QObject *o, QEvent *e);

public:
	Ui::QompPluginGettunesDlg *ui;
	AdvancedWidget<QDialog>* dialog_;
	QMenu *suggestionsMenu_;
	QTimer* sugTimer_;
	QompPluginGettunesDlg* mainDlg_;
	bool waitForSuggestions_;
};



QompPluginGettunesDlg::Private::Private(QompPluginGettunesDlg *p) :
	QObject(p),
	ui(new Ui::QompPluginGettunesDlg),
	dialog_(new AdvancedWidget<QDialog>(nullptr,  Qt::Dialog | Qt::WindowCloseButtonHint)),
	suggestionsMenu_(new QMenu(dialog_)),
	sugTimer_(new QTimer(this)),
	mainDlg_(p),
	waitForSuggestions_(false)
{
	dialog_->setAttribute(Qt::WA_DeleteOnClose);
	dialog_->setModal(true);
	ui->setupUi(dialog_);
	dialog_->setUseBorder(ThemeManager::instance()->isWindowBorderEnabled());

	connect(dialog_, SIGNAL(destroyed(QObject*)), p, SLOT(deleteLater()));

	QStringList searchHistory = Options::instance()->getOption(OPTION_SEARCH_HISTORY).toStringList();

	ui->cb_search->addItems(searchHistory);
	ui->cb_search->setInsertPolicy(QComboBox::InsertAtTop);

	ui->pb_search->setIcon(QIcon(ThemeManager::instance()->getIconFromTheme(":/icons/search")));

	sugTimer_->setSingleShot(true);
	sugTimer_->setInterval(sugTimerInterval);

	connect(sugTimer_, SIGNAL(timeout()), SLOT(timeout()));
	connect(ui->pb_search, SIGNAL(clicked()), SLOT(search()));
	connect(ui->cb_search, SIGNAL(editTextChanged(QString)), sugTimer_, SLOT(start()));

	ui->lb_busy->changeText(tr("Searching"));

	ui->cb_search->installEventFilter(this);
	suggestionsMenu_->installEventFilter(this);
	dialog_->installEventFilter(this);
	connect(suggestionsMenu_, SIGNAL(triggered(QAction*)), SLOT(suggestionActionTriggered(QAction*)));

	if(Options::instance()->getOption(OPTION_REPEAT_LAST_SEARCH).toBool()) {
		QTimer::singleShot(100, this, SLOT(search()));
	}
}

QompPluginGettunesDlg::Private::~Private()
{
	QStringList searchHistory;
	for(int i = 0; (i < ui->cb_search->count() && i < 10); i++) {
		searchHistory.append(ui->cb_search->itemText(i));
	}
	Options::instance()->setOption(OPTION_SEARCH_HISTORY, searchHistory);
	delete ui;
}

void QompPluginGettunesDlg::Private::suggestionActionTriggered(QAction *a)
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

void QompPluginGettunesDlg::Private::search()
{
	ui->cb_search->blockSignals(true);

	waitForSuggestions_ = false;
	const QString text = ui->cb_search->currentText();
	int index = ui->cb_search->findText(text);
	if(index != -1) {
		ui->cb_search->removeItem(index);
	}
	ui->cb_search->insertItem(0, text);
	ui->cb_search->setCurrentIndex(0);

	ui->cb_search->blockSignals(false);

	emit mainDlg_->doSearch(text);
}

void QompPluginGettunesDlg::Private::timeout()
{
	const QString txt = mainDlg_->currentSearchText();
	if(txt.length() > 1) {
		waitForSuggestions_ = true;
		emit mainDlg_->searchTextChanged(txt);
	}
}

bool QompPluginGettunesDlg::Private::eventFilter(QObject *o, QEvent *e)
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
	else if(o == dialog_ && e->type() == QEvent::KeyPress) {
		QKeyEvent* ke = static_cast<QKeyEvent*>(e);
		if(ke->key() == Qt::Key_Return && ui->cb_search->hasFocus()) {
			search();
			ke->accept();
			return true;
		}
	}
	return QObject::eventFilter(o, e);
}




QompPluginGettunesDlg::QompPluginGettunesDlg(QObject *parent) :
	QObject(parent)

{
	d = new Private(this);
}

QompPluginGettunesDlg::~QompPluginGettunesDlg()
{
	delete d;
	d = 0;
}

QompPluginGettunesDlg::Result QompPluginGettunesDlg::go()
{
	//We can not use here QDialog::exec() because of when
	//we use QDialog::setWindowFlags() exec call is interrupted.
	QEventLoop l;
	connect(d->dialog_, SIGNAL(finished(int)), &l, SLOT(quit()));
	d->dialog_->show();
	l.exec();
	if(d->dialog_->result() == QDialog::Accepted)
		return ResultOK;

	return ResultCancel;
}

void QompPluginGettunesDlg::setWindowTitle(const QString &title) const
{
	d->dialog_->setWindowTitle(title);
}

void QompPluginGettunesDlg::setResultsWidget(QObject *widget)
{
	QWidget *w = qobject_cast<QWidget*>(widget);
	Q_ASSERT(w);
	if(w)
		d->ui->mainLayout->insertWidget(1, w);
}

QString QompPluginGettunesDlg::currentSearchText() const
{
	return d->ui->cb_search->currentText().trimmed();
}

void QompPluginGettunesDlg::startBusyWidget()
{
	if(!d->ui->lb_busy->isActive())
		d->ui->lb_busy->start();
}

void QompPluginGettunesDlg::stopBusyWidget()
{
	d->ui->lb_busy->stop();
}

int QompPluginGettunesDlg::showAlert(const QString &title, const QString &text)
{
	return QMessageBox::critical(d->dialog_, title, text);
}

void QompPluginGettunesDlg::newSuggestions(const QStringList &list)
{
	if(!d->waitForSuggestions_)
		return;

	d->suggestionsMenu_->clear();

	foreach(const QString& sug, list) {
		QAction* act = new QAction(sug, d->suggestionsMenu_);
		d->suggestionsMenu_->addAction(act);
	}
	d->suggestionsMenu_->popup(d->dialog_->mapToGlobal(d->ui->cb_search->geometry().bottomLeft()));
	d->suggestionsMenu_->move(d->dialog_->mapToGlobal(d->ui->cb_search->geometry().bottomLeft()));
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

#include "qompplugingettunesdlg.moc"
