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

#include "yandexmusicgettunsdlg.h"
#include "qompplugintreeview.h"

class YandexMusicGettunsDlg::Private
{
public:
	explicit Private(YandexMusicGettunsDlg* p) :
		tabWidget_(new QTabWidget(0)),
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
			connect(view, SIGNAL(clicked(QModelIndex)), p, SLOT(itemSelected(QModelIndex)));
			connect(view, SIGNAL(expanded(QModelIndex)), p, SLOT(itemSelected(QModelIndex)));
		}

		connect(tabWidget_, &QTabWidget::currentChanged, p, &YandexMusicGettunsDlg::tabChanged);
	}


public:
	QTabWidget* tabWidget_;
	QompPluginTreeView *artistsView_, *albumsView_, *tracksView_;
};

YandexMusicGettunsDlg::YandexMusicGettunsDlg() :
	QompPluginGettunesDlg()
{
	setWindowTitle("Yandex.Music");

	p = new Private(this);
}

YandexMusicGettunsDlg::~YandexMusicGettunsDlg()
{
	delete p;
}

void YandexMusicGettunsDlg::setCuuretnTab(YandexMusicTabKind kind)
{
	p->tabWidget_->setCurrentIndex(int(kind));
}

void YandexMusicGettunsDlg::setModel(QAbstractItemModel *model, YandexMusicTabKind kind)
{
	switch(kind) {
	case TabAlbum:
		p->albumsView_->setModel(model);
		break;
	case TabArtist:
		p->artistsView_->setModel(model);
		break;
	case TabTrack:
		p->tracksView_->setModel(model);
		break;
	}
}

int YandexMusicGettunsDlg::currentTabRows() const
{
	return static_cast<QompPluginTreeView*>(p->tabWidget_->currentWidget())->model()->rowCount();
}

int YandexMusicGettunsDlg::currectTab() const
{
	return p->tabWidget_->currentIndex();
}
