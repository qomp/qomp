/*
 * Copyright (C) 2014  Khryukin Evgeny, Vitaly Tonkacheyev
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
#ifndef DBUSNOTIFIER_H
#define DBUSNOTIFIER_H

class QStringList;
class QImage;
class QTimer;

class DBusNotifier
{
public:
	DBusNotifier();
	~DBusNotifier();
	bool isAvailable();
	void doPopup(const QString &title, const QString &message, QImage image);
private:
	QStringList &capabilities();
	bool checkServer();
	void doCall(const QVariantList &args);
private:
	QStringList caps_;
};

#endif // DBUSNOTIFIER_H
