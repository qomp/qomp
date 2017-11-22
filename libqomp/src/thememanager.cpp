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

#include "thememanager.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QFileInfo>
#include <QResource>
#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif


static const QString iconsExpression = "Icons\\s*\\{\\s*path:\\s*(\\S+[^;]*);\\s*\\}";
static const QString nativeIconsExpression = "NativeIcons\\s*\\{\\s*([^\\}]+)\\}";
static const QString borderExpression = "UseWindowBorder\\s*\\{\\s*use:\\s*([\\d]);\\s*\\}";
static const QString themePathExpression = "<theme_path>";
static const QString resourceFileName = "*.rcc";



ThemeManager *ThemeManager::instance_ = 0;


ThemeManager *ThemeManager::instance()
{
	if(!instance_)
		instance_ = new ThemeManager;
	return instance_;
}

void ThemeManager::setTheme(const QString &theme)
{
	if(currentTheme_ == theme)
		return;

	if(!theme.isEmpty() && themes_.contains(theme)) {
		QFile file(themes_.value(theme));
		if(file.exists() && file.open(QFile::ReadOnly | QFile::Text)) {
			currentTheme_ = theme;
			prepareTheme(&file);
			emit themeChanged();
		}
	}
}

void ThemeManager::loadThemes()
{
	foreach(const QString& d, themeFolders()) {
		QDir dir(d);
		if(dir.exists()) {
			foreach(QString file, dir.entryList(QDir::Files)) {
				if(!file.endsWith(".thm"))
					continue;
				const QString path = d + '/' + file;
				file.chop(4);
				themes_.insert(file, path);
			}
		}
	}
}

void ThemeManager::prepareTheme(QFile *file)
{
	QFileInfo fi(*file);
	const QByteArray content = file->readAll().replace(themePathExpression,
					fi.absoluteDir().absolutePath().toLatin1());

	QRegExp re(iconsExpression);
	re.setMinimal(true);

	if(re.indexIn(content) != -1) {
		iconPath_ = re.cap(1);
	}
	else {
		iconPath_.clear();
	}

	re.setPattern(borderExpression);
	if(re.indexIn(content) != -1) {
		const QString u = re.cap(1);
		useBorder_ = u.compare(QStringLiteral("0")) != 0;
	}
	else {
		useBorder_ = true;
	}

	re.setPattern(nativeIconsExpression);
	if(re.indexIn(content) != -1) {
		const QString ni = re.cap(1).trimmed();
		for(const QString& line: ni.split(";", QString::SkipEmptyParts)) {
			const QStringList vals = line.split(":", QString::SkipEmptyParts);
			if(vals.count() == 2) {
				nativeIcons_.insert(vals.at(0), vals.at(1));
			}
		}
	}

	qApp->setStyleSheet(content);
}

void ThemeManager::loadExternResources()
{
	foreach(const QString& dir, themeFolders()) {
		foreach(const QString& file, QDir(dir).entryList(
						QStringList() << resourceFileName,
						QDir::Files) )
			QResource::registerResource(dir + "/" + file);
	}
}

QStringList ThemeManager::availableThemes() const
{
	return themes_.keys();
}

QString ThemeManager::getIconFromTheme(const QString &file) const
{
	if(iconPath_.isEmpty() && nativeIcons_.count() == 0)
		return file;

	QFileInfo fi(file);
	if(fi.exists()) {
		const QString fname = fi.fileName();

		if(nativeIcons_.contains(fname))
			return nativeIcons_.value(fname);

		QString ext = fi.suffix().isEmpty() ? ".png" : "";
		QString newFile = iconPath_ + "/" + fname + ext;
		if(QFileInfo::exists(newFile)) {
#ifdef DEBUG_OUTPUT
			qDebug() << "ThemeManager::getIconFromTheme()" << newFile;
#endif
			return newFile;
		}
	}

	return file;
}

bool ThemeManager::isWindowBorderEnabled() const
{
	return useBorder_;
}

ThemeManager::ThemeManager() :
	QObject(qApp),
	useBorder_(true)
{
	loadExternResources();
	loadThemes();
}

QStringList ThemeManager::themeFolders()
{
	QStringList list;
	list << ":/themes";
#ifdef Q_OS_WIN
	list.append(qApp->applicationDirPath()+"/themes");
#elif defined (HAVE_X11)
	list.append(QString(QOMP_DATADIR) + "/themes");
#elif defined (Q_OS_MAC)
	QDir appDir = qApp->applicationDirPath();
	appDir.cdUp();
	appDir.cd("Themes");
	list.append(appDir.absolutePath());
#elif defined (Q_OS_ANDROID)
	list.append("assets:/themes");
#endif
	return list;
}
