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
	QompMenu(const QString& name, QWidget *parent = nullptr);
	~QompMenu();

	virtual void buildMenu() {}

protected:
	void clearMenu();

protected slots:
	virtual void menuAboutToShow();
};

class QompGetTunesMenu : public QompMenu
{
	Q_OBJECT
public:
	QompGetTunesMenu(const QString& name, QWidget *parent = nullptr);

signals:
	void tunes(const QList<Tune*>&);

private:
	virtual void buildMenu() Q_DECL_FINAL;

private slots:
	void updateIcons();
};

class QompMainMenu : public QompMenu
{
	Q_OBJECT
public:
	explicit QompMainMenu(QWidget* p = nullptr);

	QompGetTunesMenu* tunesMenu() const;

signals:
	void actToggleVisibility();
	void actCheckUpdates();
	void actDoOptions();
	void actExit();
	void actAbout();
	void actReportBug();
	void tunes(const QList<Tune*>&);

protected slots:
	virtual void menuAboutToShow() Q_DECL_OVERRIDE;

private slots:
	void updateIcons();

private:
	virtual void buildMenu() Q_DECL_FINAL;
private:
	QompGetTunesMenu* _tunesMenu;
	QAction *_actToggle, *_actUpdates, *_actBugs, *_actQuit, *_actAboutQt, *_actOptions, *_actHome;
	QMenu *_helpMenu;
};

class QompTrackMenu : public QompMenu
{
	Q_OBJECT
public:
	QompTrackMenu(const QModelIndexList& list, QWidget* p = nullptr);

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
	virtual void buildMenu() Q_DECL_FINAL;

private:
	QModelIndexList list_;
};


class QompRemoveTunesMenu : public QompMenu
{
	Q_OBJECT
public:
	explicit QompRemoveTunesMenu(QWidget* p = nullptr);

signals:
	void removeAll();
	void removeSelected();

private:
	virtual void buildMenu() Q_DECL_FINAL;
};

#endif // QOMPMENU_H
