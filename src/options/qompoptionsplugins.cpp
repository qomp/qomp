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
#include "options.h"
#include "defines.h"

#include <QModelIndex>

#include "ui_qompoptionsplugins.h"

class QompOptionsPlugins::Private: public QObject
{
	Q_OBJECT
public:
	explicit Private(QompOptionsPlugins* page) :
		QObject(page),
		page_(page),
		ui(new Ui::QompOptionsPlugins),
		widget_(new QWidget)
	{
		ui->setupUi(widget_);
	}

	void applyOptions();
	void restoreOptions();

private slots:
	void fixSelection(const QModelIndex& parent, int start, int /*end*/);

public:
	QompOptionsPlugins* page_;
	Ui::QompOptionsPlugins *ui;
	QWidget* widget_;
};

void QompOptionsPlugins::Private::applyOptions()
{
	QStringList order;
	for(int i = 0; i < ui->tw_plugins->topLevelItemCount(); i++) {
		QTreeWidgetItem* it = ui->tw_plugins->topLevelItem(i);
		PluginManager::instance()->setPluginEnabled(it->text(0), it->data(0, Qt::CheckStateRole).toBool());
		order.append(it->text(0));
	}
	Options::instance()->setOption(OPTION_PLUGINS_ORDER, order);
	PluginManager::instance()->sortPlugins();
}

void QompOptionsPlugins::Private::restoreOptions()
{
	ui->tw_plugins->model()->disconnect(SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(fixSelection(QModelIndex,int,int)));
	ui->tw_plugins->clear();
	foreach(const QString& name, PluginManager::instance()->availablePlugins()) {
		QTreeWidgetItem* it = new QTreeWidgetItem(ui->tw_plugins);
		it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable |
			     Qt::ItemIsEnabled |
#ifdef HAVE_QT5
			     Qt::ItemNeverHasChildren |
#endif
			     Qt::ItemIsDragEnabled);
		it->setData(0, Qt::CheckStateRole, PluginManager::instance()->isPluginEnabledByName(name) ? 2 : 0);
		it->setText(0, name);
		it->setText(1, PluginManager::instance()->getVersion(name));
		it->setText(2, PluginManager::instance()->getDescription(name));
		ui->tw_plugins->addTopLevelItem(it);
	}
	ui->tw_plugins->resizeColumnToContents(0);
	connect(ui->tw_plugins->model(), SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(fixSelection(QModelIndex,int,int)));
}

void QompOptionsPlugins::Private::fixSelection(const QModelIndex& parent, int start, int /*end*/)
{
	ui->tw_plugins->setCurrentIndex(ui->tw_plugins->model()->index(start, 0, parent));
}




QompOptionsPlugins::QompOptionsPlugins(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	restoreOptions();
}

QompOptionsPlugins::~QompOptionsPlugins()
{
	delete d->ui;
	delete d;
}

void QompOptionsPlugins::retranslate()
{
	d->ui->retranslateUi(d->widget_);
}

QObject *QompOptionsPlugins::page() const
{
	return d->widget_;
}

void QompOptionsPlugins::applyOptions()
{
	d->applyOptions();
}

void QompOptionsPlugins::restoreOptions()
{
	d->restoreOptions();
}

#include "qompoptionsplugins.moc"
