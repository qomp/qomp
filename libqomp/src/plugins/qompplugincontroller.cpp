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

#include "qompplugincontroller.h"
#include "qompplugingettunesdlg.h"
#include "qompnetworkingfactory.h"
#include "tune.h"

#include <QNetworkAccessManager>

QompPluginController::QompPluginController(QObject *parent) :
	QObject(parent)
{
	nam_ = QompNetworkingFactory::instance()->getMainNAM();
}

QompPluginController::~QompPluginController()
{

}

QList<Tune*> QompPluginController::getTunes() const
{
	if(view()->go() == QompPluginGettunesDlg::ResultOK)
		return prepareTunes();

	return QList<Tune*>();
}

void QompPluginController::init()
{
	connect(view(), SIGNAL(itemSelected(QompPluginModelItem*)), SLOT(itemSelected(QompPluginModelItem*)));
	connect(view(), SIGNAL(doSearch(QString)), SLOT(doSearch(QString)));
	connect(view(), SIGNAL(searchTextChanged(QString)), SLOT(getSuggestions(QString)));
	connect(this, SIGNAL(suggestionsReady(QStringList)), view(), SLOT(newSuggestions(QStringList)));
}
