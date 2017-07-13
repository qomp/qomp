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
	virtual void doSearch(const QString& text) Q_DECL_OVERRIDE;
	virtual QompPluginGettunesDlg* view() const Q_DECL_OVERRIDE;
	virtual void itemSelected(QompPluginModelItem* item) Q_DECL_OVERRIDE;
	virtual void getSuggestions(const QString& text) Q_DECL_OVERRIDE;
	virtual void suggestionsFinished() Q_DECL_OVERRIDE;

protected:
	virtual void init() Q_DECL_OVERRIDE;
	virtual QList<Tune*> prepareTunes() const Q_DECL_OVERRIDE;

private slots:
	void searchFinished();
	void getUrlReadyRead();
	void getUrlFinished();

private:
	void doSearchStepTwo(const QString& str);
	void startBusy();
	void stopBusy();
	QString parseSearchString(QString str) const;
	void processTunes(QList<Tune*> tunes, QompPluginModelItem* item);

private:
	QompPluginTreeModel* model_;
	CuteRadioPluginGetTunesDialog* dlg_;
	int searchesCount_;
};

#endif // CUTERADIOCONTROLLER_H
