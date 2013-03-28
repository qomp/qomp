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
class QNetworkAccessManager;



class QompPluginGettunesDlg : public QDialog
{
	Q_OBJECT
	
public:
	explicit QompPluginGettunesDlg(QWidget *parent = 0);
	virtual ~QompPluginGettunesDlg();

	/**
	 * You should set results widget (like QompPluginTracksView)
	 * before use this dialog
	 */
	void setResultsWidget(QWidget* widget);

	virtual TuneList getTunes() const;
	QString currentSearchText() const;
	
protected slots:
	virtual void doSearch() = 0;

protected:
//	bool eventFilter(QObject *o, QEvent *e);
	void keyPressEvent(QKeyEvent *e);

protected:
	TuneList tunes_;
	QNetworkAccessManager* nam_;

private:
	Ui::QompPluginGettunesDlg *ui;
};

#endif // QOMPPLUGINGETTUNESDLG_H
