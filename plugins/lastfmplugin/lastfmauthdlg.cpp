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


#include "lastfmauthdlg.h"
#include "lastfmdefines.h"


#include <QDesktopServices>
#include <QUrl>

#ifndef Q_OS_ANDROID
#include "ui_lastfmauthdlg.h"

class LastFmAuthDlg::Private
{
public:
	explicit Private(LastFmAuthDlg* p) :
		parent_(p),
		ui(new Ui::LastFmAuthDlg),
		dialog_(new QDialog(nullptr, Qt::Dialog | Qt::WindowCloseButtonHint))
	{
		ui->setupUi(dialog_);
		ui->stackedWidget->setCurrentIndex(0);
		ui->busyLabel->changeText(tr("Waiting..."));

		QObject::connect(ui->buttonBox1, SIGNAL(accepted()), p, SLOT(openUrl()));
	}
	LastFmAuthDlg* parent_;
	Ui::LastFmAuthDlg *ui;
	QDialog* dialog_;
};
#else
#include <QQuickItem>
#include <QEventLoop>
#include "qompqmlengine.h"
class LastFmAuthDlg::Private
{
public:
	explicit Private(LastFmAuthDlg* p) :
		parent_(p)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/LastfmAuthDlg.qml"));
		item_->setProperty("title", LASTFM_NAME);

		QObject::connect(item_, SIGNAL(doAuth()), p, SLOT(openUrl()));
	}
	LastFmAuthDlg* parent_;
	QQuickItem* item_;
};
#endif

LastFmAuthDlg::LastFmAuthDlg(QObject *parent) :
	QObject(parent)

{
	d = new Private(this);
}

LastFmAuthDlg::~LastFmAuthDlg()
{
#ifndef Q_OS_ANDROID
	d->ui->busyLabel->stop();
	delete d->ui;
	delete d->dialog_;
#endif
	delete d;
}

LastFmAuthDlg::Result LastFmAuthDlg::openUrl(const QString &url)
{
	url_ = url;
#ifdef Q_OS_ANDROID
	QompQmlEngine::instance()->addItem(d->item_);
	QEventLoop l;
	connect(d->item_, SIGNAL(accepted()),  &l, SLOT(quit()));
	connect(d->item_, SIGNAL(destroyed()), &l, SLOT(quit()));
	l.exec();
	QompQmlEngine::instance()->removeItem();
	if(d->item_->property("status").toBool())
		return Accepted;
#else
	if(d->dialog_->exec() == QDialog::Accepted)
		return Accepted;
#endif

	return Rejected;
}

void LastFmAuthDlg::openUrl()
{
#ifndef Q_OS_ANDROID
	d->ui->stackedWidget->setCurrentIndex(1);
#endif
	QDesktopServices::openUrl(QUrl(url_));
#ifndef Q_OS_ANDROID
	d->ui->busyLabel->start();
#endif
}
