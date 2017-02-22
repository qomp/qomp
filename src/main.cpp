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

#ifdef QOMP_MOBILE
#include <QGuiApplication>
#else
#include "singleapplication/singleapplication.h"
#endif

#include "qompcon.h"
#include "defines.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setApplicationName(APPLICATION_NAME);
	QCoreApplication::setApplicationVersion(APPLICATION_VERSION);
	QCoreApplication::setOrganizationName(APPLICATION_NAME);
#ifdef QOMP_MOBILE
	QGuiApplication* a = new QGuiApplication(argc, argv);
#else
	SingleApplication* a = new SingleApplication(argc, argv, true, SingleApplication::SecondaryNotification);
#endif
	a->setQuitOnLastWindowClosed(false);
	QompCon* qomp = new QompCon;
	int ret = a->exec();
	delete qomp;
	delete a;
	return ret;
}
