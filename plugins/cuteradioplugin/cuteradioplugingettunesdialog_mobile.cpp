/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#include "cuteradioplugingettunesdialog.h"
#include "cuteradioplugindefines.h"
#include "qompqmlengine.h"
#include "qompplugintypes.h"

#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlContext>
#include <QAbstractItemModel>
#include <QQmlProperty>

class CuteRadioPluginGetTunesDialog::Private : public QObject
{
	Q_OBJECT
public:
	Private();
	~Private();
	QQuickItem* item;

signals:
	void itemClicked(const QModelIndex&);

public slots:
	void itemClicked(const QVariant &row);
};

CuteRadioPluginGetTunesDialog::Private::Private() : QObject()
{
	item = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/CuteRadioResultsView.qml"));
	connect(item, SIGNAL(checkBoxClicked(QVariant)), SLOT(itemClicked(QVariant)));
}

CuteRadioPluginGetTunesDialog::Private::~Private()
{
}

void CuteRadioPluginGetTunesDialog::Private::itemClicked(const QVariant &row)
{
	QModelIndex i = row.value<QModelIndex>();
	emit itemClicked(i);
}





CuteRadioPluginGetTunesDialog::CuteRadioPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	setWindowTitle(CUTERADIO_PLUGIN_NAME);
	p = new Private;

	connect (p, SIGNAL(itemClicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	setResultsWidget(p->item);
}

CuteRadioPluginGetTunesDialog::~CuteRadioPluginGetTunesDialog()
{
	delete p;
}

void CuteRadioPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	QQmlProperty::write(p->item, "model", QVariant::fromValue(model));
}

QString CuteRadioPluginGetTunesDialog::country() const
{
	return QString();
}

QString CuteRadioPluginGetTunesDialog::genre() const
{
	return QString();
}

void CuteRadioPluginGetTunesDialog::setCountries(QStringList *items)
{
	Q_UNUSED(items)
}

void CuteRadioPluginGetTunesDialog::setGenres(QStringList *items)
{
	Q_UNUSED(items)
}
#include "cuteradioplugingettunesdialog_mobile.moc"

