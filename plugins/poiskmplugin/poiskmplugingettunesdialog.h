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

#ifndef POISKM_PLUGIN_GETTUNESDIALOG_H
#define POISKM_PLUGIN_GETTUNESDIALOG_H

#include "qompplugingettunesdlg.h"

class QAbstractItemModel;

class PoiskmPluginGetTunesDialog : public QompPluginGettunesDlg
{
	Q_OBJECT
	
public:
	explicit PoiskmPluginGetTunesDialog(QObject *parent = nullptr);
	~PoiskmPluginGetTunesDialog();

	int page() const;
	void setPage(int page);

	int totalPages() const;
	void setTotalPages(int pages);

	void enableNext(bool enabled);
	void enablePrev(bool enabled);

	void setModel(QAbstractItemModel* model);

signals:
	void next();
	void prev();
	
private:
	class Private;
	Private* p;
	friend class Privat;
};

#endif // POISKM_PLUGIN_GETTUNESDIALOG_H
