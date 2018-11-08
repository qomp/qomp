/*
 * Copyright (C) 2014-2016  Khryukin Evgeny
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
	explicit MyzukaruController(QObject *parent = nullptr);
	~MyzukaruController();

protected slots:
	void doSearch(const QString& txt) Q_DECL_FINAL;
	QompPluginGettunesDlg* view() const Q_DECL_FINAL;
	void itemSelected(QompPluginModelItem* item) Q_DECL_FINAL;
	void getSuggestions(const QString& text) Q_DECL_FINAL;
	void suggestionsFinished() Q_DECL_FINAL;

protected:
	void init() Q_DECL_FINAL;
	QList<Tune*> prepareTunes() const Q_DECL_FINAL;

private slots:
	void searchFinished();
	void albumUrlFinished();
	void artistUrlFinished();

private:
	void checkAndStopBusyWidget();
	void getTuneUrl(QompPluginModelItem* item);

private:
	QompPluginTreeModel *tracksModel_, *albumsModel_, *artistsModel_;
	QHash<QNetworkReply*, void*> requests_;
	MyzukaruGettunesDlg* dlg_;
};

#endif // MYZUKARUCONTROLLER_H
