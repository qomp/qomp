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

#ifndef DEEZERGETTUNESDLG_H
#define DEEZERGETTUNESDLG_H

#include "qompplugingettunesdlg.h"
#include <QHash>

class Tune;
class QompPluginTreeModel;
class QTabWidget;
class QModelIndex;
class QNetworkReply;

class DeezerGettunesDlg : public QompPluginGettunesDlg
{
	Q_OBJECT
public:
	explicit DeezerGettunesDlg(QWidget *parent = 0);
	QList<Tune*> getTunes() const;

private slots:
	void artistsSearchFinished();
	void albumsSearchFinished();
	void tracksSearchFinished();
	void tuneUrlFinished();
	void albumUrlFinished();
	void artistUrlFinished();
	void itemSelected(const QModelIndex& ind);
	void getSuggestions(const QString& text);
	void suggestionsFinished();
	void doSearchTunes(const QString& txt);

private:
	void checkAndStopBusyWidget();
	void search(const QString& text, const QString& type, const char* slot, int page = 1);
	void searchNextPage(const QString& reply, const QString& type, const char* slot);

private:
	QompPluginTreeModel *tracksModel_, *albumsModel_, *artistsModel_;
	QHash<QNetworkReply*, void*> requests_;
	QTabWidget* tabWidget_;
	QString userToken_;
};

#endif // DEEZRGETTUNESDLG_H
