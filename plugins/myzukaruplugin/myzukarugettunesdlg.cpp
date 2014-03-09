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

#include "myzukarugettunesdlg.h"
#include "qompplugintreeview.h"
#include "myzukarudefines.h"


//#include <QDebug>


MyzukaruGettunesDlg::MyzukaruGettunesDlg(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	tabWidget_(new QTabWidget(this)),
	artistsView_(new QompPluginTreeView(this)),
	albumsView_(new QompPluginTreeView(this)),
	tracksView_(new QompPluginTreeView(this))
{
	setWindowTitle(MYZUKA_PLUGIN_NAME);

	tabWidget_->addTab(artistsView_, tr("Artists"));
	tabWidget_->addTab(albumsView_, tr("Albums"));
	tabWidget_->addTab(tracksView_, tr("Tracks"));

	setResultsWidget(tabWidget_);

	QList<QompPluginTreeView*> list = QList<QompPluginTreeView*> () << artistsView_ << albumsView_ << tracksView_;
	foreach(QompPluginTreeView* view, list) {
		connect(view, SIGNAL(clicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
		connect(view, SIGNAL(expanded(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	}
}

void MyzukaruGettunesDlg::setModel(QAbstractItemModel *model, MyzikaruTabKind kind)
{
	switch (kind) {
	case TabAlbums:
		albumsView_->setModel(model);
		break;
	case TabArtists:
		artistsView_->setModel(model);
		break;
	case TabTracks:
		tracksView_->setModel(model);
		break;
	}
}

void MyzukaruGettunesDlg::setCurrentTab(MyzikaruTabKind kind)
{
	tabWidget_->setCurrentIndex(int(kind));
}
