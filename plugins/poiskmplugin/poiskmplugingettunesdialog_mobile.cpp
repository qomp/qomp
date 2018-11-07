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
#include "qompqmlengine.h"
#include "qompplugintypes.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QAbstractItemModel>
#include <QQmlProperty>

class PoiskmPluginGetTunesDialog::Private : public QObject
{
	Q_OBJECT
public:
	Private();
	~Private();
	QQuickItem* item;

signals:
	void itemClicked(const QModelIndex&);
	void next();

public slots:
	void itemClicked(const QVariant &row);
};

PoiskmPluginGetTunesDialog::Private::Private() : QObject()
{
	item = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/PoiskmResultView.qml"));
	connect(item, SIGNAL(itemCheckClick(QVariant)), SLOT(itemClicked(QVariant)));
	connect(item, SIGNAL(actNext()), SIGNAL(next()));
}

PoiskmPluginGetTunesDialog::Private::~Private()
{
}

void PoiskmPluginGetTunesDialog::Private::itemClicked(const QVariant &row)
{
	QModelIndex i = row.value<QModelIndex>();
	emit itemClicked(i);
}





PoiskmPluginGetTunesDialog::PoiskmPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	setWindowTitle(POISKM_PLUGIN_NAME);
	p = new Private;

	connect (p, SIGNAL(itemClicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	connect (p, &PoiskmPluginGetTunesDialog::Private::next, this, &PoiskmPluginGetTunesDialog::next);
	setResultsWidget(p->item);
}

PoiskmPluginGetTunesDialog::~PoiskmPluginGetTunesDialog()
{
	delete p;
}

int PoiskmPluginGetTunesDialog::page() const
{
	return QQmlProperty::read(p->item, "page").toInt();
}

void PoiskmPluginGetTunesDialog::setPage(int page)
{
	QQmlProperty::write(p->item, "page", page);
}

int PoiskmPluginGetTunesDialog::totalPages() const
{
	return 0;//return ui->lb_pages->text().toInt();
}

void PoiskmPluginGetTunesDialog::setTotalPages(int /*p*/)
{
	//ui->lb_pages->setText(QString::number(p));
}

void PoiskmPluginGetTunesDialog::enableNext(bool enabled)
{
	QQmlProperty::write(p->item, "nextEnabled", enabled);
}

void PoiskmPluginGetTunesDialog::enablePrev(bool/* enabled*/)
{
	//QQmlProperty::write(p->item, "prevEnabled", enabled);
}

void PoiskmPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	QQmlProperty::write(p->item, "model", QVariant::fromValue(model));
}

#include "poiskmplugingettunesdialog_mobile.moc"
