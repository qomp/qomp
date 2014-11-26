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

#ifndef MYZUKARUCONTROLLER_H
#define MYZUKARUCONTROLLER_H

#include "qompplugincontroller.h"

#include <QHash>

class MyzukaruGettunesDlg;
class QompPluginTreeModel;
class QNetworkReply;
class QompPluginModelItem;

class MyzukaruController : public QompPluginController
{
	Q_OBJECT
public:
	explicit MyzukaruController(QObject *parent = 0);
	virtual ~MyzukaruController();

protected slots:
	void doSearch(const QString& txt);
	QompPluginGettunesDlg* view() const;
	void itemSelected(QompPluginModelItem* item);
	void getSuggestions(const QString& text);
	void suggestionsFinished();

protected:
	void init();
	QList<Tune*> prepareTunes() const;

private slots:
	void searchFinished();
	//void tuneUrlFinished();
	void albumUrlFinished();
	void artistUrlFinished();
	void tuneUrlFinished();

private:
	void checkAndStopBusyWidget();

private:
	QompPluginTreeModel *tracksModel_, *albumsModel_, *artistsModel_;
	QHash<QNetworkReply*, void*> requests_;
	MyzukaruGettunesDlg* dlg_;
};

#endif // MYZUKARUCONTROLLER_H
