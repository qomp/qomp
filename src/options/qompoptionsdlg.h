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

#ifndef QOMPOPTIONSDLG_H
#define QOMPOPTIONSDLG_H

#include <QDialog>

namespace Ui {
class QompOptionsDlg;
}
class QompOptionsPage;
class QAbstractButton;
class QompMainWin;
class QompPlayer;

class QompOptionsDlg : public QDialog
{
	Q_OBJECT
	
public:
	QompOptionsDlg(QompPlayer* player, QompMainWin *parent = 0);
	~QompOptionsDlg();

public slots:
	virtual void accept();

protected:
	void changeEvent(QEvent *e);
	void keyReleaseEvent(QKeyEvent* ke);

private slots:
	void applyOptions();
	void itemChanged(int row);
	void buttonClicked(QAbstractButton* b);
	void pageDestroyed();
	void pluginLoadingStatusChanged(const QString& pluginName, bool status);

private:
	void addPluginPage(const QString& name);

	
private:
	Ui::QompOptionsDlg *ui;
//	QList<QompOptionsPage*> pages_;
};

#endif // QOMPOPTIONSDLG_H
