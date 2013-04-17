/*
 * Copyright (C) 2013  Khryukin Evgeny
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

#include "common.h"
#include <QTime>
#include <QTextDocument>
#ifdef HAVE_QT5
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif
#include <QDir>

namespace Qomp {

QString encodePassword(const QString &pass, const QString &key)
{
	QString result;
	int n1, n2;

	if (key.length() == 0) {
		return pass;
	}

	for (n1 = 0, n2 = 0; n1 < pass.length(); ++n1) {
		ushort x = pass.at(n1).unicode() ^ key.at(n2++).unicode();
		QString hex;
		hex.sprintf("%04x", x);
		result += hex;
		if(n2 >= key.length()) {
			n2 = 0;
		}
	}
	return result;
}

QString decodePassword(const QString &pass, const QString &key)
{
	QString result;
	int n1, n2;

	if (key.length() == 0) {
		return pass;
	}

	for(n1 = 0, n2 = 0; n1 < pass.length(); n1 += 4) {
		ushort x = 0;
		if(n1 + 4 > pass.length()) {
			break;
		}
		x += QString(pass.at(n1)).toInt(NULL,16)*4096;
		x += QString(pass.at(n1+1)).toInt(NULL,16)*256;
		x += QString(pass.at(n1+2)).toInt(NULL,16)*16;
		x += QString(pass.at(n1+3)).toInt(NULL,16);
		QChar c(x ^ key.at(n2++).unicode());
		result += c;
		if(n2 >= key.length()) {
			n2 = 0;
		}
	}
	return result;
}


QString durationSecondsToString(uint sec)
{
	int h = sec / (60*60);
	sec -= h*(60*60);
	int m = sec / 60;
	sec -= m*60;
	QTime time(h, m, sec, 0);
	const QString format = h ? "hh:mm:ss" : "mm:ss";
	return time.toString(format);
}

QString durationMiliSecondsToString(qint64 ms)
{
	int h = ms / (1000*60*60);
	ms -= h*(1000*60*60);
	int m = ms / (1000*60);
	ms -= m*(1000*60);
	int s = ms / 1000;
	ms -= s*1000;
	QTime t(h, m, s, ms);
	const QString format = h ? "hh:mm:ss" : "mm:ss";
	return t.toString(format);
}

QString unescape(const QString& escaped)
{
	QTextDocument doc;
	doc.setHtml(escaped);
//	QString plain = escaped;
//	plain.replace("&lt;", "<");
//	plain.replace("&gt;", ">");
//	plain.replace("&quot;", "\"");
//	plain.replace("&amp;", "&");
//	plain.replace("&#39;", "'");
	return doc.toPlainText();
}

QString cacheDir()
{
	QString dir;
#ifdef HAVE_QT5
	QStringList list = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if(!list.isEmpty())
		dir = list.first();
	else
		dir = QDir::homePath();
#else
	dir = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
#endif
	QDir d(dir);
	if(!d.exists())
		d.mkdir(d.path());

	return dir;
}

QString dataDir()
{
	QString dir;
#ifdef HAVE_QT5
	QStringList list = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
	if(!list.isEmpty())
		dir = list.first();
	else
		dir = QDir::homePath();
#else
	dir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
	return dir;
}

} //namespace Qomp
