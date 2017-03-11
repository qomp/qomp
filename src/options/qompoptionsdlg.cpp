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

#include "qompoptionsdlg.h"
#include "qompoptionspage.h"
#include "pluginmanager.h"
#include "qompoptionsmain.h"
#include "options.h"
#include "qompoptionsplugins.h"
#include "qompoptionskeys.h"
#include "advwidget.h"
#include "thememanager.h"

#include "ui_qompoptionsdlg.h"

#include <QAbstractButton>
#include <QListWidgetItem>
#include <QDialog>

class QompOptionsDlg::Private: public QObject
{
	Q_OBJECT
public:
	explicit Private(QompOptionsDlg* p) :
		QObject(p),
		parentDialog_(p),
		dlg_(new AdvancedWidget<QDialog>(nullptr, Qt::Dialog | Qt::WindowCloseButtonHint)),
		ui(new Ui::QompOptionsDlg)
	{
		dlg_->setAttribute(Qt::WA_DeleteOnClose);
		dlg_->setModal(true);
		ui->setupUi(dlg_);
		dlg_->setUseBorder(ThemeManager::instance()->isWindowBorderEnabled());

		connect(dlg_, SIGNAL(destroyed(QObject*)), p, SLOT(deleteLater()));

		QompOptionsMain* om = new QompOptionsMain(this);
		QompOptionsPlugins* op = new QompOptionsPlugins(this);
		QompOptionsKeys *ok = new QompOptionsKeys(this);
		pages_ << om << op << ok;

		foreach(QompOptionsPage* page, pages_) {
			addPage(page);
		}

		foreach(const QString& p, PluginManager::instance()->availablePlugins()) {
			addPluginPage(p);
		}

		ui->sw_pages->setCurrentIndex(0);
		ui->lw_pagesNames->setCurrentRow(0);

		connect(ui->lw_pagesNames, SIGNAL(currentRowChanged(int)), SLOT(itemChanged(int)));
		connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(buttonClicked(QAbstractButton*)));

		connect(PluginManager::instance(), SIGNAL(pluginStatusChanged(QString, bool)), parentDialog_, SLOT(pluginLoadingStatusChanged(QString,bool)));

		dlg_->adjustSize();
		ui->lw_pagesNames->setFixedWidth(ui->lw_pagesNames->width());
		dlg_->installEventFilter(this);

		connect(dlg_, SIGNAL(accepted()), parentDialog_, SLOT(applyOptions()));
	}

	void addPluginPage(const QString& name)
	{
		QompOptionsPage *p = PluginManager::instance()->getOptions(name);
		if(p) {
			pages_.append(p);
			addPage(p);
		}
	}

protected:
	bool eventFilter(QObject *o, QEvent *e)
	{
		if(o == dlg_ && e->type() == QEvent::LanguageChange) {
			ui->retranslateUi(dlg_);
			for(int i = 0; i < pages_.count(); i++) {
				QompOptionsPage* p = pages_.at(i);
				p->retranslate();
				QListWidgetItem* it = ui->lw_pagesNames->item(i);
				it->setText(p->name());
			}
		}
		return QObject::eventFilter(o, e);
	}

private slots:
	void buttonClicked(QAbstractButton* b)
	{
		if(ui->buttonBox->standardButton(b) == QDialogButtonBox::Apply) {
			parentDialog_->applyOptions();
		}
	}

	void itemChanged(int row)
	{
		ui->sw_pages->setCurrentIndex(row);
		pages_.at(row)->restoreOptions();
	}

	void pageDestroyed()
	{
		QompOptionsPage* page = static_cast<QompOptionsPage*>(sender());
		int ind = pages_.indexOf(page);
		QWidget* w = ui->sw_pages->widget(ind);
		ui->sw_pages->removeWidget(w);
		delete w;
		QListWidgetItem* it = ui->lw_pagesNames->item(ind);
		ui->lw_pagesNames->removeItemWidget(it);
		pages_.removeAll(page);
		delete it;
	}

private:
	void addPage(QompOptionsPage* page)
	{
		page->init(parentDialog_->player_);
		QWidget* widget = qobject_cast<QWidget*>(page->page());
		Q_ASSERT(widget);
		ui->sw_pages->addWidget(widget);
		QListWidgetItem* it = new QListWidgetItem(ui->lw_pagesNames);
		it->setText(page->name());
		ui->lw_pagesNames->addItem(it);
		connect(page, SIGNAL(destroyed()), SLOT(pageDestroyed()));
	}

public:
	QompOptionsDlg* parentDialog_;
	AdvancedWidget<QDialog>* dlg_;
	Ui::QompOptionsDlg *ui;
	QList<QompOptionsPage*> pages_;
};




QompOptionsDlg::QompOptionsDlg(QompPlayer *player, QObject *parent) :
	QObject(parent),
	player_(player)
{
	d = new Private(this);
}

QompOptionsDlg::~QompOptionsDlg()
{
	delete d->ui;
}

void QompOptionsDlg::exec()
{
	d->dlg_->show();
}

void QompOptionsDlg::applyOptions()
{
	for(int i=0; i<d->pages_.count(); ++i) {
		d->pages_.at(i)->applyOptions();
	}

	Options::instance()->applyOptions();
}

void QompOptionsDlg::pluginLoadingStatusChanged(const QString &pluginName, bool status)
{
	if(status)
		addPluginPage(pluginName);
}

void QompOptionsDlg::addPluginPage(const QString &name)
{
	d->addPluginPage(name);
}

#include "qompoptionsdlg.moc"
