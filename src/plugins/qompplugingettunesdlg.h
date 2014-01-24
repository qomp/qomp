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

#ifndef QOMPPLUGINGETTUNESDLG_H
#define QOMPPLUGINGETTUNESDLG_H

#include "tune.h"

#include <QDialog>

namespace Ui {
class QompPluginGettunesDlg;
}

class QMenu;
class QompPluginModelItem;


class QompPluginGettunesDlg : public QDialog
{
	Q_OBJECT
	
public:
	explicit QompPluginGettunesDlg(QWidget *parent = 0);
	virtual ~QompPluginGettunesDlg();

	enum Result { ResultOK, ResultCancel };

	virtual Result go();

	/**
	 * Call this to start busy widget
	 */
	void startBusyWidget();

	/**
	 * Call this to stop busy widget
	 */
	void stopBusyWidget();

signals:
	/**
	 * The signal is emited when text in search combobox changed by user
	 */
	void searchTextChanged(const QString&);
	void doSearch(const QString&);
	void itemSelected(QompPluginModelItem* item);
	
public slots:
	/**
	 * Call this slot when list of suggestions received
	 */
	void newSuggestions(const QStringList& list);

protected:
	void keyPressEvent(QKeyEvent *e);
	bool eventFilter(QObject *o, QEvent *e);

	/**
	 * You should set results widget (like QompPluginTreeView)
	 * before use this dialog
	 */
	void setResultsWidget(QWidget* widget);

	/**
	 * Return current text in combo box
	 */
	QString currentSearchText() const;

protected slots:
	void itemSelected(const QModelIndex& ind);

private slots:
	void suggestionActionTriggered(QAction* a);
	void search();

private:
	Ui::QompPluginGettunesDlg *ui;
	QMenu *suggestionsMenu_;
};

#endif // QOMPPLUGINGETTUNESDLG_H
