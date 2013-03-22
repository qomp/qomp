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

#ifndef GETTUNESDIALOG_H
#define GETTUNESDIALOG_H

#include <QDialog>

#include "tune.h"

namespace Ui {
class ProstoPleerPluginGetTunesDialog;
}
class QNetworkAccessManager;
class QNetworkReply;
class ProstopleerModel;
class QModelIndex;
class QEvent;

class ProstoPleerPluginGetTunesDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ProstoPleerPluginGetTunesDialog(QWidget *parent = 0);
	~ProstoPleerPluginGetTunesDialog();

	TuneList getTunes() const;

public slots:
	virtual void accept();

private slots:
	void doSearch();
	void searchFinished();
	void urlFinished();
	void loginFinished();
	void itemSelected(const QModelIndex& index);
	void itemActivated(const QModelIndex& index);

	void actPrevActivated();
	void actNextActivated();
	void doSettings();

protected:
	bool eventFilter(QObject *o, QEvent *e);
	void keyPressEvent(QKeyEvent *e);

private:
	void doLogin();
	void doSearchStepTwo();
	
private:
	Ui::ProstoPleerPluginGetTunesDialog *ui;
	TuneList tunes_;
	QNetworkAccessManager* nam_;
	ProstopleerModel* model_;
	QString lastSearchStr_;
};

#endif // GETTUNESDIALOG_H
