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

#ifndef PROSTOPLEER_PLUGIN_GETTUNESDIALOG_H
#define PROSTOPLEER_PLUGIN_GETTUNESDIALOG_H

#include "qompplugingettunesdlg.h"

#include "tune.h"

namespace Ui {
class ProstoPleerPluginResultsWidget;
}

class QompPluginModelItem;
class QompPluginTreeModel;
class QModelIndex;

class ProstoPleerPluginGetTunesDialog : public QompPluginGettunesDlg
{
	Q_OBJECT
	
public:
	explicit ProstoPleerPluginGetTunesDialog(QWidget *parent = 0);
	~ProstoPleerPluginGetTunesDialog();

	TuneList getTunes() const;

public slots:
	virtual void accept();

protected slots:
	void doSearch();

private slots:
	void searchFinished();
	void urlFinished();
	void loginFinished();
	void itemSelected(const QModelIndex& index);
	void searchSuggestions(const QString& text);
	void suggestionsFinished();

	void actPrevActivated();
	void actNextActivated();

private:
	void doLogin();
	void doSearchStepTwo();
	
private:
	Ui::ProstoPleerPluginResultsWidget *ui;
	TuneList tunes_;
	QompPluginTreeModel* model_;
	QString lastSearchStr_;
};

#endif // GETTUNESDIALOG_H
