/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#ifndef PROSTOPLEERPLUGINSETTINGS_H
#define PROSTOPLEERPLUGINSETTINGS_H

#include "qompoptionspage.h"

class ProstopleerPluginSettings : public QompOptionsPage
{
	Q_OBJECT	
public:
	explicit ProstopleerPluginSettings(QObject *parent = 0);
	~ProstopleerPluginSettings();

	virtual QString name() const;
	virtual void retranslate();
	virtual QObject* page() const;

public slots:
	virtual void applyOptions();
	virtual void restoreOptions();
	
private:
	class Private;
	Private* d;
};

#endif // PROSTOPLEERPLUGINSETTINGS_H
