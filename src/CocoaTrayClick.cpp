/*
 * CocoaTrayClick
 * Copyright (C) 2012, 2015  Khryukin Evgeny
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "CocoaTrayClick.h"
#include <objc/runtime.h>
#include <objc/message.h>
#include <QApplication>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

bool dockClickHandler(id /*self*/, SEL /*_cmd*/, ...)
{
	CocoaTrayClick::instance()->emitTrayClicked();
	return true;
}


CocoaTrayClick * CocoaTrayClick::instance()
{
	if(!instance_)
		instance_ = new CocoaTrayClick();

	return instance_;
}

CocoaTrayClick::CocoaTrayClick()
	: QObject(qApp)
{
	Class cls = objc_getClass("NSApplication");
	objc_object *appInst = objc_msgSend((objc_object*)cls, sel_registerName("sharedApplication"));

	if(appInst != NULL) {
		objc_object* delegate = objc_msgSend(appInst, sel_registerName("delegate"));
		Class delClass = (Class)objc_msgSend(delegate,  sel_registerName("class"));
		SEL shouldHandle = sel_registerName("applicationShouldHandleReopen:hasVisibleWindows:");
		if (class_getInstanceMethod(delClass, shouldHandle)) {
			if (class_replaceMethod(delClass, shouldHandle, (IMP)dockClickHandler, "B@:")) {
#ifdef DEBUG_OUTPUT
				qDebug() << "Registered dock click handler (replaced original method)";
#endif
			}
			else {
#ifdef DEBUG_OUTPUT
				qDebug() << "Failed to replace method for dock click handler";
#endif
			}
		}
		else {
			if (class_addMethod(delClass, shouldHandle, (IMP)dockClickHandler,"B@:")) {
#ifdef DEBUG_OUTPUT
				qDebug() << "Registered dock click handler";
#endif
			}
			else {
#ifdef DEBUG_OUTPUT
				qDebug() << "Failed to register dock click handler";
#endif
			}
		}
	}
}

CocoaTrayClick::~CocoaTrayClick()
{
}

void CocoaTrayClick::emitTrayClicked()
{
	emit trayClicked();
}

CocoaTrayClick* CocoaTrayClick::instance_ = NULL;
