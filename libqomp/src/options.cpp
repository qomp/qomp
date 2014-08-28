/*
 * Copyright (C) 2011-2013  Khryukin Evgeny
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


#include "options.h"
#include "defines.h"
#include <QSettings>
#include <QCoreApplication>


Options* Options::instance_ = 0;

Options* Options::instance()
{
	if(!instance_) {
		instance_ = new Options();
	}

	return instance_;
}

Options::Options()
	: QObject(QCoreApplication::instance())
	, set_(0)
{
#ifdef Q_OS_ANDROID
	set_ = new QSettings(QString("/sdcard/.%1/%2.ini")
			     .arg(qApp->organizationName(), qApp->applicationName()),
			     QSettings::IniFormat, this);
#else
	set_ = new QSettings(QSettings::IniFormat, QSettings::UserScope,
			     qApp->organizationName(), qApp->applicationName(), this);
#endif
}

Options::~Options()
{
	set_->sync();
}

void Options::reset()
{
	delete instance_;
	instance_ = 0;
}

QVariant Options::getOption(const QString& name, const QVariant& defValue)
{
	QVariant val = set_->value(name, defValue);
	return val;
}

void Options::setOption(const QString& name, const QVariant& value)
{
	set_->setValue(name, value);
#ifdef Q_OS_ANDROID
	set_->sync();
#endif
}

void Options::applyOptions()
{
	set_->sync();
	emit updateOptions();
}
