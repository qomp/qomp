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

#ifndef MYZUKARUGETTUNESDLG_H
#define MYZUKARUGETTUNESDLG_H

#include "qompplugingettunesdlg.h"
#include <QHash>

class QTabWidget;
class QAbstractItemModel;
class QompPluginTreeView;

enum MyzikaruTabKind { TabArtists = 0, TabAlbums = 1, TabTracks = 2 };

class MyzukaruGettunesDlg : public QompPluginGettunesDlg
{
	Q_OBJECT
	
public:
	explicit MyzukaruGettunesDlg(QWidget *parent = 0);

	void setModel(QAbstractItemModel* model, MyzikaruTabKind kind);
	void setCurrentTab(MyzikaruTabKind kind);

private:
	QTabWidget* tabWidget_;
	QompPluginTreeView *artistsView_, *albumsView_, *tracksView_;
};

#endif // MYZUKARUGETTUNESDLG_H
