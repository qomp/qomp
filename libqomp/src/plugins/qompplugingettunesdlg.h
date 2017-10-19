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

#include "common.h"
#include "libqomp_global.h"

#include <QObject>

class QompPluginModelItem;
class QModelIndex;
class QStyle;


class LIBQOMPSHARED_EXPORT QompPluginGettunesDlg : public QObject
{
	Q_OBJECT
	
public:
	explicit QompPluginGettunesDlg(QObject *parent = 0);
	virtual ~QompPluginGettunesDlg();

	enum class Result { ResultOK, ResultCancel };

	/**
	 * wrapper to internal dialog exec() call
	 */
	virtual void go();

	/**
	 * set internal dialo title
	 */
	void setWindowTitle(const QString& title) const;

	/**
	 * Call this to start busy widget
	 */
	void startBusyWidget();

	/**
	 * Call this to stop busy widget
	 */
	void stopBusyWidget();

	int showAlert(const QString& title, const QString& text);

signals:
	/**
	 * The signal is emited when text in search combobox changed by user
	 */
	void searchTextChanged(const QString&);
	void doSearch(const QString&);
	void itemSelected(QompPluginModelItem* item);
	void finished(Result);
	
public slots:
	/**
	 * Call this slot when list of suggestions received
	 */
	void newSuggestions(const QStringList& list);

protected:
	/**
	 * You should set results widget (like QompPluginTreeView)
	 * before use this dialog
	 */
	void setResultsWidget(QObject* widget);

	/**
	 * Return current text in combo box
	 */
	QString currentSearchText() const;

protected slots:
	void itemSelected(const QModelIndex& ind);

private:
	class Private;
	Private* d;
	friend class Private;
};

#endif // QOMPPLUGINGETTUNESDLG_H
