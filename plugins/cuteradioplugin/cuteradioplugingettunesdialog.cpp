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


#include <QKeyEvent>

#include "qompplugintreeview.h"

class CuteRadioPluginGetTunesDialog::Private
{
public:
	Private() : _view(new QompPluginTreeView) {}

	QompPluginTreeView* _view;
};


CuteRadioPluginGetTunesDialog::CuteRadioPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	p = new Private;

	setWindowTitle(CUTERADIO_PLUGIN_NAME);
	setResultsWidget(p->_view);

	connect(p->_view, SIGNAL(itemActivated(QModelIndex)), SLOT(itemSelected(QModelIndex)));
}

CuteRadioPluginGetTunesDialog::~CuteRadioPluginGetTunesDialog()
{
	delete p;
}

void CuteRadioPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	p->_view->setModel(model);
}
