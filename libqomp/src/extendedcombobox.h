/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#ifndef EXTENDEDCOMBOBOX_H
#define EXTENDEDCOMBOBOX_H

#include <QComboBox>

#include "libqomp_global.h"

class LIBQOMPSHARED_EXPORT ExtendedComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(bool headerHidden READ isHeaderHidden WRITE setHeaderHidden)
public:
	explicit ExtendedComboBox(QWidget* parent = nullptr);

	void setHeaderHidden(bool hide);
	bool isHeaderHidden() const;

	void setHorizontalHeaderLabels(const QStringList &labels);
	void addItem(const QVariantList &item);

private:
	class Private;
	Private* d;
	friend class Private;
};

#endif // EXTENDEDCOMBOBOX_H
