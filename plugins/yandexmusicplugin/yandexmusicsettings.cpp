/*
 * Copyright (C) 2021  Khryukin Evgeny
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

#include "yandexmusicsettings.h"
#include "options.h"
#include "common.h"
#include "yandexmusicoauth.h"
#include "yandexmusicplugin.h"

#ifndef Q_OS_ANDROID
#include "ui_yandexmusicsettings.h"
#else
#include "qompqmlengine.h"
#include <QQuickItem>
#endif
#include <QDateTime>

#ifndef Q_OS_ANDROID
class YandexMusicSettings::Private
{
public:
	explicit Private(YandexMusicSettings* p) :
		page_(p),
		widget_(new QWidget),
		ui(new Ui::YandexMusicSettings)
	{
		ui->setupUi(widget_);
		ui->lbBusy->changeText(QObject::tr("Executing the request..."));
		QObject::connect(ui->pbAuthentication, &QPushButton::clicked, page_, &YandexMusicSettings::doAuth);
		QObject::connect(ui->pbAuthentication, &QPushButton::clicked, ui->lbBusy, &QompBusyLabel::start);
	}

	~Private()
	{
		delete ui;
	}

	YandexMusicSettings* page_;
	QWidget* widget_;
	Ui::YandexMusicSettings* ui;

#else
class YandexMusicSettings::Private
{
public:
	explicit Private(YandexMusicSettings* p) :
		page_(p)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/YandexMusicOptionsPage.qml"));
		QObject::connect(item_, SIGNAL(doAuth()), page_, SLOT(doAuth()));
	}

	YandexMusicSettings* page_;
	QQuickItem* item_;

#endif

	QString userName() const
	{
#ifndef Q_OS_ANDROID
		return ui->leUser->text();
#else
		return item_->property("login").toString();
#endif
	}

	QString password() const
	{
#ifndef Q_OS_ANDROID
		return ui->lePassword->text();
#else
		return item_->property("password").toString();
#endif
	}

	void setResult(const QString& result, bool ok = true)
	{
#ifndef Q_OS_ANDROID
		Q_UNUSED(ok)
		ui->lbResult->setText(result);
		ui->lbBusy->stop();
#else
		if(ok) {
			item_->setProperty("result", result);
		}
		else {
			QMetaObject::invokeMethod(item_, "showError", Q_ARG(QVariant, QVariant::fromValue(result)));
		}
#endif
	}
};

YandexMusicSettings::YandexMusicSettings(QObject *parent) :
	QompOptionsPage(parent),
	_auth(new YandexMusicOauth(this))
{
	d = new Private(this);
	connect(this, &YandexMusicSettings::authRequest, _auth, &YandexMusicOauth::grant);
	connect(_auth, &YandexMusicOauth::granted, [&]() {
		restoreOptions();
		d->setResult(tr("Authentication succeeded."));
	});
	connect(_auth, &YandexMusicOauth::requestError, [&](const QString& error) {
		restoreOptions();
		d->setResult(error, false);
	});
	restoreOptions();
}

QString YandexMusicSettings::name() const
{
	return qobject_cast<YandexMusicPlugin*>(parent())->name();
}

YandexMusicSettings::~YandexMusicSettings()
{
	delete d;
}

void YandexMusicSettings::retranslate()
{
#ifndef Q_OS_ANDROID
	d->ui->retranslateUi(d->widget_);
#endif
}

QObject *YandexMusicSettings::page() const
{
#ifndef Q_OS_ANDROID
	return d->widget_;
#else
	return d->item_;
#endif
}

void YandexMusicSettings::doAuth()
{
	emit authRequest(d->userName(), d->password());
}

void YandexMusicSettings::applyOptions()
{
}

void YandexMusicSettings::restoreOptions()
{
	QString status;
	if(YandexMusicOauth::token().size() > 0) {
		status = tr("Authenticated up to ") + YandexMusicOauth::tokenTtl().toString(Qt::SystemLocaleShortDate);
	}
	else {
		status = tr("Not authenticated");
	}
#ifndef Q_OS_ANDROID
	d->ui->leUser->setText(_auth->userName());
	d->ui->lePassword->setText("");
	d->ui->lbStatus->setText(status);
#else
	d->item_->setProperty("login", _auth->userName());
	d->item_->setProperty("password", "");
	d->item_->setProperty("status", status);
#endif
}
