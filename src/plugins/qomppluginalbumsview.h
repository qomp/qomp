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

#ifndef QOMPPLUGINALBUMSVIEW_H
#define QOMPPLUGINALBUMSVIEW_H

#include <QTreeView>
#include <QTime>

class QompPluginTune;
struct QompPluginAlbum;

class QompPluginAlbumsView : public QTreeView
{
	Q_OBJECT
public:
	QompPluginAlbumsView(QWidget *parent = 0);
	
protected:
	void keyPressEvent(QKeyEvent *ke);
	void mousePressEvent(QMouseEvent *e);
	void mouseDoubleClickEvent(QMouseEvent *e);

signals:
	void tuneSelected(QompPluginTune*);
	void albumSelected(QompPluginAlbum*);

private slots:
	void itemSelected();

private:
	void itemActivated();

private:
	QTime lastClickTime_;
};

#endif // QOMPPLUGINALBUMSVIEW_H
