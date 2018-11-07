/*
 * Copyright (C) 2013-2018  Khryukin Evgeny
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

#include "qompoptionsdlg.h"
#include "qompoptionspage.h"
#include "pluginmanager.h"
#include "qompoptionsmain.h"
#include "options.h"
#include "qompqmlengine.h"
#include "defines.h"
#include "qompoptionsplugins.h"

#include <QQuickItem>
#include <QQmlProperty>

class QompOptionsDlg::Private: public QObject
{
	Q_OBJECT
public:
	explicit Private(QompOptionsDlg* p) :
		QObject(p),
		parentDialog_(p),
		item_(0)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/OptionsDlg.qml"));
		item_->setProperty("qompVer", QString(APPLICATION_NAME) + " " + QString(APPLICATION_VERSION));
		QompOptionsMain* om = new QompOptionsMain(this);
		QompOptionsPlugins* op = new QompOptionsPlugins(this);
		pages_ << om << op;

		foreach(QompOptionsPage* page, pages_) {
			addPage(page);
		}

		foreach(const QString& p, PluginManager::instance()->availablePlugins()) {
			addPluginPage(p);
		}

		connect(PluginManager::instance(), SIGNAL(pluginStatusChanged(QString, bool)), parentDialog_,
			SLOT(pluginLoadingStatusChanged(QString,bool)));
	}

	void addPluginPage(const QString& name)
	{
		QompOptionsPage *p = PluginManager::instance()->getOptions(name);
		if(p) {
			pages_.append(p);
			addPage(p);
		}
	}

public slots:
	void dlgAccepted() {
		parentDialog_->applyOptions();
		QompQmlEngine::instance()->removeItem();
		parentDialog_->deleteLater();
	}

private slots:
	void pageDestroyed()
	{
		QompOptionsPage* page = static_cast<QompOptionsPage*>(sender());
		pages_.removeAll(page);
	}

private:
	void addPage(QompOptionsPage* page)
	{
		page->init(parentDialog_->player_);
		QQuickItem* item = qobject_cast<QQuickItem*>(page->page());
		Q_ASSERT(item);

		item->setParent(0);
		QompQmlEngine::instance()->setObjectOwnership(item, QQmlEngine::JavaScriptOwnership);
		QMetaObject::invokeMethod(item_, "addPage",
					  Q_ARG(QVariant, page->name()),
					  Q_ARG(QVariant, QVariant::fromValue(item)));

		connect(page, SIGNAL(destroyed()), SLOT(pageDestroyed()));
	}

public:
	QompOptionsDlg* parentDialog_;
	QList<QompOptionsPage*> pages_;
	QQuickItem* item_;
};




QompOptionsDlg::QompOptionsDlg(QompPlayer *player, QObject *parent) :
	QObject(parent),
	player_(player)
{
	d = new Private(this);
}

QompOptionsDlg::~QompOptionsDlg()
{
	//delete d->item_;
}

void QompOptionsDlg::exec()
{
	connect(d->item_, SIGNAL(accepted()), d, SLOT(dlgAccepted()));
	QompQmlEngine::instance()->addItem(d->item_);
}

void QompOptionsDlg::applyOptions()
{
	for(int i=0; i<d->pages_.count(); ++i) {
		d->pages_.at(i)->applyOptions();
	}

	Options::instance()->applyOptions();
}

void QompOptionsDlg::pluginLoadingStatusChanged(const QString &pluginName, bool status)
{
	if(status)
		addPluginPage(pluginName);
}

void QompOptionsDlg::addPluginPage(const QString &name)
{
	d->addPluginPage(name);
}

#include "qompoptionsdlg_mobile.moc"
