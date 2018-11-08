/*
 * Copyright (C) 2016  Khryukin Evgeny
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

#ifndef QOMPPLUGINCAPTCHADIALOG_H
#define QOMPPLUGINCAPTCHADIALOG_H

#include <QObject>
#include "libqomp_global.h"

class QPixmap;

class LIBQOMPSHARED_EXPORT QompPluginCaptchaDialog: public QObject
{
	Q_OBJECT
public:
	QompPluginCaptchaDialog(const QPixmap& captcha, QObject* parent = nullptr);
	~QompPluginCaptchaDialog();

	bool start();
	QString result() const;

private:
	class Private;
	Private* d;
};

#endif // QOMPPLUGINCAPTCHADIALOG_H
