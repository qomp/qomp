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

#ifndef QOMPPLUGINCONTROLLER_H
#define QOMPPLUGINCONTROLLER_H

#include <QObject>

#include "tune.h"

class QNetworkAccessManager;
class QompPluginModelItem;
class QompPluginGettunesDlg;

class QompPluginController : public QObject
{
	Q_OBJECT
public:
	explicit QompPluginController(QObject *parent = 0);
	virtual ~QompPluginController();

	/**
	 * Return list of Tunes
	 */
	virtual TuneList getTunes() const = 0;

signals:
	void suggestionsReady(const QStringList&);

protected slots:
	virtual QompPluginGettunesDlg* view() const = 0;
	/**
	 * This slot is called when Search button is clicked
	 * or Enter key pressed
	 */
	virtual void doSearch(const QString& text) = 0;

	virtual void itemSelected(QompPluginModelItem* item) = 0;

	virtual void getSuggestions(const QString& text) = 0;
	virtual void suggestionsFinished() = 0;

protected:
	QNetworkAccessManager* nam() const { return nam_; }
	virtual void init();

private:
	QNetworkAccessManager* nam_;
};

#endif // QOMPPLUGINCONTROLLER_H
