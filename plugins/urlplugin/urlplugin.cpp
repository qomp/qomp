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

#include "urlplugin.h"
#include "tune.h"
#include "qomppluginaction.h"

#ifndef Q_OS_ANDROID
#include <QInputDialog>
#else
#include "qompqmlengine.h"
#include <QEventLoop>
#include <QQuickItem>
#endif
#include <QtPlugin>

UrlPlugin::UrlPlugin()
{
}

QList<Tune*> UrlPlugin::getTunes()
{
	QList<Tune*> list;
#ifndef Q_OS_ANDROID
	bool ok = false;
	QString url = QInputDialog::getText(0, tr("Input url"), "URL:",QLineEdit::Normal, "", &ok);
	if(ok && !url.isEmpty()) {
		list.append(urlToTune(url));
	}
#else
	QEventLoop l;
	QQuickItem *item = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/GetUrlDlg.qml"));
	item->setProperty("title",PLUGIN_NAME);
	connect(item, SIGNAL(accepted()), &l, SLOT(quit()));
	connect(item, SIGNAL(destroyed()), &l, SLOT(quit()));
	QompQmlEngine::instance()->addItem(item);
	l.exec();
	if(item->property("status").toBool()) {
		QString url = item->property("url").toString();
		if(!url.isEmpty()) {
			list.append(urlToTune(url));
		}
	}
	QompQmlEngine::instance()->removeItem();
#endif

	return list;
}

Tune *UrlPlugin::urlToTune(const QString &url)
{
	Tune *tune = new Tune;
	tune->url = url;
	return tune;
}

QompOptionsPage *UrlPlugin::options()
{
	return 0;
}

QList<QompPluginAction *> UrlPlugin::getTunesActions()
{
	QList<QompPluginAction *> l;
	QompPluginAction *act = new QompPluginAction(QIcon(), tr("Url"), this, "getTunes", this);
	l.append(act);
	return l;
}

bool UrlPlugin::processUrl(const QString &url, QList<Tune *> *tunes)
{
	QUrl u(url);
	if(u.isValid() && !u.isLocalFile()) {
		tunes->append(urlToTune(u.toString()));
		return true;
	}

	return false;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(urlplugin, UrlPlugin)
#endif
