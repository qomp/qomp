/*
 * growlnotifier.mm - A simple Qt interface to Growl
 *
 * Copyright (C) 2005  Remko Troncon
 *               2012  Khryukin Evgeny
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * You can also redistribute and/or modify this program under the
 * terms of the Psi License, specified in the accompanied COPYING
 * file, as published by the Psi Project; either dated January 1st,
 * 2005, or (at your option) any later version.
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

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>
#include <Growl/GrowlApplicationBridge.h>

#include <QBuffer>
#include "growlnotifier.h"


NSString* qString2NSString(const QString& str)
{
	return  [ NSString stringWithUTF8String: str.toUtf8().constData() ];
}

NSArray *qStringList2NSArray(const QStringList& list)
{
	int cnt = list.count();
	if(!cnt)
		return nil;

	NSString *strings[cnt];
	for(int i = 0; i < cnt; i++) {
		strings[i] = qString2NSString(list.at(i));
	}
	return [ NSArray arrayWithObjects: strings count: cnt ];
}

NSData *qPixmap2NSData(const QPixmap& p)
{
	if (p.isNull()) {
		return nil;
	}

	QByteArray img_data;
	QBuffer buffer(&img_data);
	buffer.open(QIODevice::WriteOnly);
	p.save(&buffer, "PNG");
	return [ NSData dataWithBytes: img_data.constData() length: img_data.size() ];
}

@interface GrowlController : NSObject <GrowlApplicationBridgeDelegate> {
@private
	NSArray* allNotifications;
	NSArray* defaultNotifications;
	NSString* appName;
}
-(void) setAllNotifications:(NSArray*)all setDefaultNotifications:(NSArray*) def setName:(NSString*) name;
@end

@implementation GrowlController

-(id) init {
	self = [super init];
	if(self) {
		allNotifications = nil;
		defaultNotifications = nil;
		appName = nil;
	}
	return self;
}

-(void)dealloc {
	if(allNotifications)
		[allNotifications release];
	if(defaultNotifications)
		[defaultNotifications release];
	if(appName)
		[appName release];
	[super dealloc];
}

-(void) setAllNotifications:(NSArray*)all setDefaultNotifications:(NSArray*) def setName:(NSString*) name {
	allNotifications = all;
	defaultNotifications = def;
	appName = name;
	if(allNotifications)
		[allNotifications retain];
	if(defaultNotifications)
		[defaultNotifications retain];
	if(appName)
		[appName retain];
}

-(NSDictionary*) registrationDictionaryForGrowl {

	NSDictionary *dict = [ NSDictionary dictionaryWithObjectsAndKeys:
			allNotifications, GROWL_NOTIFICATIONS_ALL,
			defaultNotifications, GROWL_NOTIFICATIONS_DEFAULT,
			nil ];
	return dict;
}

-(NSString*) applicatioNameForGrowl {
	return appName;
}

-(NSData*) applicationIconDataForGrowl {
	return nil;
}

-(void) growlNotificationWasClicked:(id)clickContext {
}

-(void) growlNotificationTimedOut:(id)clickContext {
}

@end

//------------------------------------------------------------------------------ 
class GrowlNotifier::Private
{
public:
	Private(){}
	~Private() {}

	GrowlController* controller_;
};


/**
 * Constructs a GrowlNotifier.
 *
 * \param notifications the list names of all notifications that can be sent
 *	by this notifier.
 * \param default_notifications the list of names of the notifications that
 *  should be enabled by default.
 * \param app the name of the application under which the notifier should
 *	register with growl.
 */
GrowlNotifier::GrowlNotifier(const QStringList& notifications,
			     const QStringList& default_notifications,
			     const QString& app)
{
	d = new Private;
	d->controller_ = [ [ GrowlController alloc ] init ];
	[ d->controller_ setAllNotifications: qStringList2NSArray(notifications)
	      setDefaultNotifications: qStringList2NSArray(default_notifications)
	      setName: qString2NSString(app) ];

	[ GrowlApplicationBridge setGrowlDelegate: d->controller_ ];
}

GrowlNotifier::~GrowlNotifier()
{
	[ d->controller_ release ];
	delete d;
}

/**
 * \brief Sends a notification to Growl.
 *
 * \param name the registered name of the notification.
 * \param title the title for the notification.
 * \param description the description of the notification.
 * \param icon the icon of the notification.
 * \param sticky whether the notification should be sticky (i.e. require a
 *	click to discard.
 */
void GrowlNotifier::notify(const QString& name, const QString& title, 
			   const QString& description, const QPixmap& p, bool sticky)
{
	[ GrowlApplicationBridge notifyWithTitle: qString2NSString(title)
	      description: qString2NSString(description)
	      notificationName: qString2NSString(name)
	      iconData: qPixmap2NSData(p)
	      priority: 0
	      isSticky: sticky ? YES : NO
	      clickContext: nil ];
}

bool GrowlNotifier::isRunning()
{
	return [ GrowlApplicationBridge isGrowlRunning ];
}
