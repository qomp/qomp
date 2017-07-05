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

#ifndef CUTERADIOCONTROLLER_H
#define CUTERADIOCONTROLLER_H

#include "qompplugincontroller.h"

class QompPluginModelItem;
class CuteRadioPluginGetTunesDialog;
class QompPluginTreeModel;

class CuteRadioController : public QompPluginController
{
	Q_OBJECT
public:
	explicit CuteRadioController(QObject *parent = 0);
	virtual ~CuteRadioController();

protected slots:
	void doSearch(const QString& text);
	QompPluginGettunesDlg* view() const;
	void itemSelected(QompPluginModelItem* item);
	void getSuggestions(const QString& text);
	void suggestionsFinished();

protected:
	void init();
	QList<Tune*> prepareTunes() const;

private slots:
	void searchFinished();

private:
	void doSearchStepTwo(const QString& str);
	void startBusy();
	void stopBusy();
	QString parseSearchString(QString str) const;

private:
	QompPluginTreeModel* model_;
	CuteRadioPluginGetTunesDialog* dlg_;
	int searchesCount_;
};

#endif // CUTERADIOCONTROLLER_H
