/*
 * Copyright (C) 2014, 2016  Khryukin Evgeny
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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QMap>

#include "libqomp_global.h"

class QFile;

class LIBQOMPSHARED_EXPORT ThemeManager : public QObject
{
	Q_OBJECT
public:
	static ThemeManager* instance();

	void setTheme(const QString& theme);
	QStringList availableThemes() const;
	QString getIconFromTheme(const QString& file) const;
	bool isWindowBorderEnabled() const;

signals:
	void themeChanged();

private:
	ThemeManager();

	void loadThemes();
	void prepareTheme(QFile* file);
	void loadExternResources();

	static QStringList themeFolders();

private:
	static ThemeManager* instance_;
	QMap<QString,QString> themes_;
	QString currentTheme_;
	QString iconPath_;
	bool useBorder_;
};

#endif // THEMEMANAGER_H
