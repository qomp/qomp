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

#ifndef QOMPACTIONSLIST_H
#define QOMPACTIONSLIST_H

#include <QObject>
#include <QMap>
#include <QKeySequence>

#include "libqomp_global.h"

class LIBQOMPSHARED_EXPORT QompActionsList : public QObject
{
	Q_OBJECT
public:
	static QompActionsList* instance();

	enum QompActionType {
		ActPlay,
		ActStop,
		ActNext,
		ActPrevious,
		ActOpen,
		ActClearPlaylist,
		ActSettings,
		ActRepeatAll,
		ActMute,
		ActShuffle
	};

	struct QompAction {
		QompAction(QompActionType _type, const QString& _name, const QKeySequence& _shortcut) :
			type(_type), name(_name), shortcut(_shortcut)
		{}

		QompActionType type;
		QString name;
		QKeySequence shortcut;

		bool operator==(const QompAction& other) const
		{
			return type == other.type;
		}
	};

	QKeySequence getShortcut(QompActionType type) const;
	void updateAction(QompActionType type, const QKeySequence& key);
	const QList<QompAction> actions() const;

	void saveToOptions();
	void restoreFromOptions();

private:
	void fillActionsList();

private:
	QompActionsList();
	static QompActionsList* instance_;
	QMap<QString, QompAction> actions_;
};

#endif // QOMPACTIONSLIST_H
