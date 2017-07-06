/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "qompplugingettunesdlg.h"
#include "qompplugintreemodel.h"
#include "defines.h"
#include "options.h"
#include "tune.h"
#include "qompqmlengine.h"

#include <QTimer>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlComponent>
#include <QEventLoop>
#include <QQmlProperty>

static const int sugTimerInterval = 1000;


class QompPluginGettunesDlg::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QompPluginGettunesDlg* p = 0);
	~Private();

public slots:
	void search();
	void timeout();

public:
	QQuickItem* item_;
	QStringList searchHistory_;
	QTimer* sugTimer_;
	QompPluginGettunesDlg* mainDlg_;
	QEventLoop* loop_;
};

QompPluginGettunesDlg::Private::Private(QompPluginGettunesDlg *p) :
	QObject(p),
	item_(0),
	sugTimer_(new QTimer(this)),
	mainDlg_(p),
	loop_(new QEventLoop(this))
{
	item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qmlshared/GetTunesDlg.qml"));

	searchHistory_ = Options::instance()->getOption(OPTION_SEARCH_HISTORY).toStringList();
	QQmlProperty::write(item_, "model", QVariant::fromValue(searchHistory_));

	sugTimer_->setSingleShot(true);
	sugTimer_->setInterval(sugTimerInterval);

	connect(item_, SIGNAL(doSearch()), SLOT(search()));
	connect(item_, SIGNAL(accepted()), loop_, SLOT(quit()));
	connect(item_, SIGNAL(destroyed()), loop_, SLOT(quit()));
	connect(item_, SIGNAL(editTextChanged()), sugTimer_, SLOT(start()));
	connect(sugTimer_, SIGNAL(timeout()), SLOT(timeout()));
}

QompPluginGettunesDlg::Private::~Private()
{
	Options::instance()->setOption(OPTION_SEARCH_HISTORY, searchHistory_);
	QompQmlEngine::instance()->removeItem();
	//item_->deleteLater();
}

void QompPluginGettunesDlg::Private::search()
{
	QString text = mainDlg_->currentSearchText();
	if(text.isEmpty())
		return;

	int index = searchHistory_.indexOf(text);
	if(index != -1) {
		searchHistory_.removeAt(index);
	}
	searchHistory_.insert(0, text);
	while(searchHistory_.length() > 10)
		searchHistory_.takeLast();

	QQmlProperty::write(item_, "model", QVariant::fromValue(searchHistory_));

	emit mainDlg_->doSearch(text);
}

void QompPluginGettunesDlg::Private::timeout()
{
	QString text = mainDlg_->currentSearchText();
	if(text.length() > 2) {
		emit mainDlg_->searchTextChanged(text);
		QQmlProperty::write(item_, "waitForSuggestions", true);
	}
}




QompPluginGettunesDlg::QompPluginGettunesDlg(QObject *parent) :
	QObject(parent)

{
	d = new Private(this);
}

QompPluginGettunesDlg::~QompPluginGettunesDlg()
{
	delete d;
	d = 0;
}

QompPluginGettunesDlg::Result QompPluginGettunesDlg::go()
{
	QompQmlEngine::instance()->addItem(d->item_);
	d->loop_->exec();
	d->loop_->disconnect();
	if(d->item_->property("status").toBool())
		return ResultOK;

	return ResultCancel;
}

void QompPluginGettunesDlg::setWindowTitle(const QString &title) const
{
	d->item_->setProperty("title", title);
}

void QompPluginGettunesDlg::setResultsWidget(QObject *widget)
{
	QQuickItem* w = qobject_cast<QQuickItem*>(widget);
	Q_ASSERT(w);
	if(w) {
		QObjectList l;
		l.append(w);
		QObject* content = d->item_->property("pluginContent").value<QObject*>();
		Q_ASSERT(content);
		w->setParent(content);
		QQmlProperty::write(w, "parent", QVariant::fromValue(content));
	}
}

QString QompPluginGettunesDlg::currentSearchText() const
{
	return d->item_->property("serchText").toString().trimmed();
}

void QompPluginGettunesDlg::startBusyWidget()
{
	QQmlProperty::write(d->item_, "busy", true);
}

void QompPluginGettunesDlg::stopBusyWidget()
{
	QQmlProperty::write(d->item_, "busy", false);
}

void QompPluginGettunesDlg::newSuggestions(const QStringList &list)
{
	QStringList l(list);
	while(l.size() > 8)
		l.takeLast();

	QMetaObject::invokeMethod(d->item_, "doSuggestions", Q_ARG(QVariant, QVariant::fromValue(l)));
}

void QompPluginGettunesDlg::itemSelected(const QModelIndex &ind)
{
	if(!ind.isValid())
		return;

	const QompPluginTreeModel* model = qobject_cast<const QompPluginTreeModel*>(ind.model());
	QompPluginModelItem* item = model->item(ind);

	emit itemSelected(item);
}

int QompPluginGettunesDlg::showAlert(const QString &title, const QString &text)
{
	QMetaObject::invokeMethod(d->item_, "showAlert",
				  Q_ARG(QVariant, title),
				  Q_ARG(QVariant, text));
	return 0;
}

#include "qompplugingettunesdlg_mobile.moc"
