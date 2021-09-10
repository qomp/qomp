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

#include "yandexmusicsettings.h"
#include "options.h"
#include "common.h"
#include "yandexmusicoauth.h"
#include "yandexmusicplugin.h"

#include <QDateTime>
#ifndef Q_OS_ANDROID
#include "ui_yandexmusicsettings.h"

class YandexMusicSettings::Private
{
public:
	explicit Private(YandexMusicSettings* p) :
		page_(p),
		widget_(new QWidget),
		ui(new Ui::YandexMusicSettings)
	{
		ui->setupUi(widget_);
		QObject::connect(ui->pb_authentication, &QPushButton::clicked, page_, &YandexMusicSettings::doAuth);
	}

	~Private()
	{
		delete ui;
	}

	YandexMusicSettings* page_;
	QWidget* widget_;
	Ui::YandexMusicSettings* ui;

#else
#include "qompqmlengine.h"
#include <QQuickItem>

class YandexMusicSettings::Private
{
public:
	explicit Private(YandexMusicSettings* p) :
		page_(p)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/YandexMusicOptionsPage.qml"));
		QObject::connect(item_, SIGNAL(clicked()), page_, SIGNAL(doAuth()));
	}

	YandexMusicSettings* page_;
	QQuickItem* item_;

#endif

	QString userName() const
	{
#ifndef Q_OS_ANDROID
		return ui->leUser->text();
#else
		return "";
#endif
	}

	QString password() const
	{
#ifndef Q_OS_ANDROID
		return ui->lePassword->text();
#else
		return "";
#endif
	}

	void setResult(const QString& result)
	{
#ifndef Q_OS_ANDROID
		ui->lbResult->setText(result);
#else
		Q_UNUSED(result)
#endif
	}
};

YandexMusicSettings::YandexMusicSettings(QObject *parent) :
	QompOptionsPage(parent),
	_auth(new YandexMusicOauth(this))
{
	d = new Private(this);
	connect(this, &YandexMusicSettings::authRequest, _auth, &YandexMusicOauth::grant);
	QObject::connect(_auth, &YandexMusicOauth::granted, [&]() {
		restoreOptions();
		d->setResult(tr("Authentication succeeded."));
	});
	connect(_auth, &YandexMusicOauth::requestError, [&](const QString& error) {
		restoreOptions();
		d->setResult(error);
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
#ifndef Q_OS_ANDROID
	d->ui->leUser->setText(_auth->userName());
	d->ui->lePassword->setText("");
	if(YandexMusicOauth::token().size() > 0) {
		d->ui->lbStatus->setText(tr("Authenticated up to ") + YandexMusicOauth::tokenTtl().toString(Qt::SystemLocaleShortDate));
	}
	else {
		d->ui->lbStatus->setText(tr("Not authenticated"));
	}
#else
	d->item_->setProperty("login", _auth->user());
#endif
}
