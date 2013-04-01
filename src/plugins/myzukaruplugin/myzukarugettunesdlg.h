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

class QompPluginTune;
class QompPluginAlbum;
class QompPluginTracksView;
class QompPluginAlbumsView;

class MyzukaruGettunesDlg : public QompPluginGettunesDlg
{
	Q_OBJECT
	
public:
	explicit MyzukaruGettunesDlg(QWidget *parent = 0);

public slots:
	virtual void accept();

protected:
//	bool eventFilter(QObject *o, QEvent *e);

protected slots:
	virtual void doSearch();

private slots:
	void searchFinished();
	void tuneUrlFinished();
	void albumUrlFinished();
	void tuneSelected(QompPluginTune* tune);
	void albumSelected(QompPluginAlbum* album);

private:
	void addTune(QompPluginTune* tune);

private:
	QompPluginAlbumsView* albumsView_;
	QompPluginAlbumsView* artistsView_;
	QompPluginTracksView* tracksView_;
};

#endif // MYZUKARUGETTUNESDLG_H
