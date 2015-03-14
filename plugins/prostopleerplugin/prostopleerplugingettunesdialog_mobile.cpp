/*
 * Copyright (C) 2014  Khryukin Evgeny
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
#include "qompqmlengine.h"
#include "qompplugintypes.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QAbstractItemModel>
#include <QQmlProperty>

class ProstoPleerPluginGetTunesDialog::Private : public QObject
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

ProstoPleerPluginGetTunesDialog::Private::Private() : QObject()
{
	item = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/ProstoPleerResultView.qml"));
	connect(item, SIGNAL(itemCheckClick(QVariant)), SLOT(itemClicked(QVariant)));
	connect(item, SIGNAL(actNext()), SIGNAL(next()));
}

ProstoPleerPluginGetTunesDialog::Private::~Private()
{
}

void ProstoPleerPluginGetTunesDialog::Private::itemClicked(const QVariant &row)
{
	//QAbstractItemModel *model = item->property("model").value<QAbstractItemModel*>();
	QModelIndex i = row.value<QModelIndex>();
	//model->setData(i, QompCon::DataToggle, Qt::CheckStateRole);
	emit itemClicked(i);
}





ProstoPleerPluginGetTunesDialog::ProstoPleerPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	setWindowTitle(PROSTOPLEER_PLUGIN_NAME);
	p = new Private;

	connect (p, SIGNAL(itemClicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	connect (p, SIGNAL(next()), SIGNAL(next()));
	setResultsWidget(p->item);
}

ProstoPleerPluginGetTunesDialog::~ProstoPleerPluginGetTunesDialog()
{
	delete p;
}

void ProstoPleerPluginGetTunesDialog::setAuthStatus(const QString &status)
{
	QQmlProperty::write(p->item, "auth", status);
}

int ProstoPleerPluginGetTunesDialog::page() const
{
	return QQmlProperty::read(p->item, "page").toInt();
}

void ProstoPleerPluginGetTunesDialog::setPage(int page)
{
	QQmlProperty::write(p->item, "page", page);
}

int ProstoPleerPluginGetTunesDialog::totalPages() const
{
	return 0;//return ui->lb_pages->text().toInt();
}

void ProstoPleerPluginGetTunesDialog::setTotalPages(int /*p*/)
{
	//ui->lb_pages->setText(QString::number(p));
}

void ProstoPleerPluginGetTunesDialog::enableNext(bool enabled)
{
	QQmlProperty::write(p->item, "nextEnabled", enabled);
}

void ProstoPleerPluginGetTunesDialog::enablePrev(bool/* enabled*/)
{
	//QQmlProperty::write(p->item, "prevEnabled", enabled);
}

void ProstoPleerPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	QQmlProperty::write(p->item, "model", QVariant::fromValue(model));
}

#include "prostopleerplugingettunesdialog_mobile.moc"

