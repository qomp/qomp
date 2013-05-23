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

#include "qomptrayicon.h"
#include "defines.h"

#include <QWheelEvent>


QompTrayIcon::QompTrayIcon(QObject *parent) :
	QObject(parent),
	icon_(new QSystemTrayIcon( this))
{
	setIcon(QIcon(":/icons/icons/qomp_stop.png"));
	icon_->setToolTip(QString("%1 %2").arg(APPLICATION_NAME, APPLICATION_VERSION));
	connect(icon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	icon_->installEventFilter(this);
	icon_->show();
}

void QompTrayIcon::setToolTip(const QString &text)
{
	if(text.isEmpty())
		icon_->setToolTip(QString("%1 %2").arg(APPLICATION_NAME, APPLICATION_VERSION));
	else
		icon_->setToolTip(QString("%1 %2\n%3").arg(APPLICATION_NAME, APPLICATION_VERSION, text));
}

void QompTrayIcon::setIcon(const QIcon &ico)
{
	icon_->setIcon(ico);
}


void QompTrayIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch(reason) {
	case QSystemTrayIcon::DoubleClick:
		emit trayDoubleClicked();
		break;
	case QSystemTrayIcon::MiddleClick:
		emit trayClicked(Qt::MidButton);
		break;
	case QSystemTrayIcon::Context:
		emit trayClicked(Qt::RightButton);
		break;
	case QSystemTrayIcon::Trigger:
		emit trayClicked(Qt::LeftButton);
		break;
	default:
		break;
	}
}

bool QompTrayIcon::eventFilter(QObject *o, QEvent *e)
{
	if(o == icon_ && e->type() == QEvent::Wheel) {
		QWheelEvent *we = static_cast<QWheelEvent*>(e);
		emit trayWheeled(we->delta());
	}
	return QObject::eventFilter(o, e);
}
