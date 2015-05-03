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

#include "myzukarugettunesdlg.h"
#include "myzukarudefines.h"
#include "qompplugintreeview.h"

#include <QTabWidget>


#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

class MyzukaruGettunesDlg::Private
{
public:
	explicit Private(MyzukaruGettunesDlg* p) :
		tabWidget_(new QTabWidget),
		artistsView_(new QompPluginTreeView(tabWidget_)),
		albumsView_(new QompPluginTreeView(tabWidget_)),
		tracksView_(new QompPluginTreeView(tabWidget_))
	{
		tabWidget_->addTab(artistsView_, tr("Artists"));
		tabWidget_->addTab(albumsView_, tr("Albums"));
		tabWidget_->addTab(tracksView_, tr("Tracks"));

		p->setResultsWidget(tabWidget_);

		QList<QompPluginTreeView*> list = QList<QompPluginTreeView*> () << artistsView_ << albumsView_ << tracksView_;
		foreach(QompPluginTreeView* view, list) {
			QObject::connect(view, SIGNAL(clicked(QModelIndex)), p, SLOT(itemSelected(QModelIndex)));
			QObject::connect(view, SIGNAL(expanded(QModelIndex)), p, SLOT(itemSelected(QModelIndex)));
		}

	}
	QTabWidget* tabWidget_;
	QompPluginTreeView *artistsView_, *albumsView_, *tracksView_;
};




MyzukaruGettunesDlg::MyzukaruGettunesDlg(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	p = new Private(this);
	setWindowTitle(MYZUKA_PLUGIN_NAME);
}

MyzukaruGettunesDlg::~MyzukaruGettunesDlg()
{
	delete p;
}

void MyzukaruGettunesDlg::setModel(QAbstractItemModel *model, MyzikaruTabKind kind)
{
	switch (kind) {
	case TabAlbums:
		p->albumsView_->setModel(model);
		break;
	case TabArtists:
		p->artistsView_->setModel(model);
		break;
	case TabTracks:
		p->tracksView_->setModel(model);
		break;
	}
}

void MyzukaruGettunesDlg::setCurrentTab(MyzikaruTabKind kind)
{
	p->tabWidget_->setCurrentIndex(int(kind));
}
