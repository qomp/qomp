/*
 * Copyright (C) 2013  Khryukin Evgeny, Vitaly Tonkacheyev
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

#include "mpriscontroller.h"

#include <QtDBus/QDBusConnection>

MprisController::MprisController(QObject *parent) :
	QObject(parent)
{
	mprisAdapter_ = new MprisAdapter(this);
	QDBusConnection qompConnection = QDBusConnection::sessionBus();
	qompConnection.registerObject("/org/mpris/MediaPlayer2", this);
	qompConnection.registerService("org.mpris.MediaPlayer2.qomp");
}

MprisController::~MprisController()
{
	QDBusConnection::sessionBus().unregisterService("org.mpris.MediaPlayer2.qomp");
}

void MprisController::sendData(const QString &status, const QompMetaData &tune)
{
	mprisAdapter_->setStatus(status);
	mprisAdapter_->setMetadata(tune);
	mprisAdapter_->updateProperties();
}
