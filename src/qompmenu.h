/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#ifndef QOMPMENU_H
#define QOMPMENU_H

#include <QMenu>
#include <QAbstractItemModel>

class Tune;

class QompMenu : public QMenu
{
	Q_OBJECT
protected:
	explicit QompMenu(QWidget *parent = 0);
	QompMenu(const QString& name, QWidget *parent = 0);

	virtual void buildMenu() {}

private slots:
	void menuAboutToShow();
};

class QompGetTunesMenu : public QompMenu
{
	Q_OBJECT
public:
	explicit QompGetTunesMenu(QWidget *parent = 0);
	QompGetTunesMenu(const QString& name, QWidget *parent = 0);
	~QompGetTunesMenu();

signals:
	void tunes(const QList<Tune*>&);

private slots:
	void actionActivated(QAction *sender);

private:
	void buildMenu();
	void init();
};

class QompMainMenu : public QompMenu
{
	Q_OBJECT
public:
	explicit QompMainMenu(QWidget* p = 0);

signals:
	void actToggleVisibility();
	void actCheckUpdates();
	void actDoOptions();
	void actExit();
	void actAbout();
	void actReportBug();
	void tunes(const QList<Tune*>&);

private:
	void buildMenu();
};

class QompTrackMenu : public QompMenu
{
	Q_OBJECT
public:
	QompTrackMenu(const QModelIndexList& list, QWidget* p = 0);

signals:
	void togglePlayState(Tune*);
	void removeTune(const QModelIndexList&);
	void saveTune(const QModelIndexList&);
	void copyUrl(Tune*);

private slots:
	void actRemoveActivated();
	void actCopyUrlActivated();
	void actSaveActivated();
	void actToggleActivated();
	void actOpenDirectActivated();

private:
	void buildMenu();

private:
	QModelIndexList list_;
};


class QompRemoveTunesMenu : public QompMenu
{
	Q_OBJECT
public:
	explicit QompRemoveTunesMenu(QWidget* p = 0);

signals:
	void removeAll();
	void removeSelected();

private:
	void buildMenu();
};

#endif // QOMPMENU_H
