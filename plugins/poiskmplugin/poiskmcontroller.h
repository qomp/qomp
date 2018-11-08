/*
 * Copyright (C) 2018  Khryukin Evgeny
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

#ifndef POISKMCONTROLLER_H
#define POISKMCONTROLLER_H

#include "qompplugincontroller.h"

class QompPluginModelItem;
class PoiskmPluginGetTunesDialog;
class QompPluginTreeModel;

class PoiskmController : public QompPluginController
{
	Q_OBJECT
public:
	explicit PoiskmController(QObject *parent = nullptr);
	~PoiskmController();

protected slots:
	virtual void doSearch(const QString& text) Q_DECL_OVERRIDE;
	virtual QompPluginGettunesDlg* view() const Q_DECL_OVERRIDE;
	virtual void itemSelected(QompPluginModelItem* item) Q_DECL_OVERRIDE;
	virtual void getSuggestions(const QString& text) Q_DECL_OVERRIDE;
	virtual void suggestionsFinished() Q_DECL_OVERRIDE;

protected:
	virtual void init() Q_DECL_FINAL;
	virtual QList<Tune*> prepareTunes() const Q_DECL_OVERRIDE;

private slots:
	void actPrevActivated();
	void actNextActivated();
	void searchFinished();

private:
	void doSearchStepTwo();
	void startBusy();
	void stopBusy();

private:
	QompPluginTreeModel* model_;
	QString lastSearchStr_;
	PoiskmPluginGetTunesDialog* dlg_;
	int searchesCount_;
};

#endif // POISKMCONTROLLER_H
