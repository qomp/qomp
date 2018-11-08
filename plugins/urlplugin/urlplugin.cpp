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
#include <QQuickItem>
#endif
#include <QtPlugin>
#include <QFileInfo>


class UrlPlugin::Private: public QObject
{
	Q_OBJECT
public:
	explicit Private(QompPluginAction* p) : QObject(p), _act(p)
	{
#ifdef Q_OS_ANDROID
		_item = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/GetUrlDlg.qml"));
		_item->setProperty("title",PLUGIN_NAME);
		connect(_item, SIGNAL(accepted()), SLOT(dlgFinished()));
#endif
	}

	void start()
	{
#ifdef Q_OS_ANDROID
		QompQmlEngine::instance()->addItem(_item);
#else
		bool ok = false;
		QString url = QInputDialog::getText(0, tr("Input url"), "URL:",QLineEdit::Normal, "", &ok);
		if(ok) {
			_url = url;
		}
		dlgFinished();
#endif
	}

	static Tune* urlToTune(const QString& url)
	{
		Tune *tune = new Tune;
		tune->url = url;
		return tune;
	}

public slots:
	void dlgFinished()
	{
		QList<Tune*> list;
#ifdef Q_OS_ANDROID
		if(_item->property("status").toBool()) {
			QString url = _item->property("url").toString();
			if(!url.isEmpty()) {
				list.append(urlToTune(url));
			}
		}
		QompQmlEngine::instance()->removeItem();
#else
		if(!_url.isEmpty())
			list.append(urlToTune(_url));
#endif
		_act->setTunesReady( list );

		deleteLater();
	}

private:
	QompPluginAction* _act;
#ifdef Q_OS_ANDROID
	QQuickItem *_item;
#else
	QString _url = "";
#endif
};

UrlPlugin::UrlPlugin() : d(nullptr)
{
}

void UrlPlugin::getTunes(QompPluginAction *act)
{
	d = new Private(act);
	d->start();
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
	if(u.isValid() && !u.isLocalFile() && !QFileInfo(url).exists()) {
		tunes->append(Private::urlToTune(u.toString()));
		return true;
	}

	return false;
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(urlplugin, UrlPlugin)
#endif


#include "urlplugin.moc"
