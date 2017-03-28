/*
 * Copyright (C) 2014  Khryukin Evgeny, Vitaly Tonkacheyev
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
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QImage>
#include <QSize>
#include <QStringList>

#include "dbusnotifier.h"
#include "defines.h"

static const int minLifeTime = 5000;
static const QString markupCaps = "body-markup";
static const QSize defaultSize = QSize(48,48);
static const QString TITLE_BEGIN = "<b><u>";
static const QString TITLE_END = "</u></b>";

class iiibiiay
{
public:
	explicit iiibiiay(QImage* img)
	{
		if(img->format() != QImage::Format_ARGB32)
			*img = img->convertToFormat(QImage::Format_ARGB32);
		width = img->width();
		height = img->height();
		rowstride = img->bytesPerLine();
		hasAlpha = img->hasAlphaChannel();
		channels = img->isGrayscale()?1:hasAlpha?4:3;
		bitsPerSample = img->depth()/channels;
		image.append((char*)img->rgbSwapped().bits(),img->byteCount());
	}
	iiibiiay(){}
	static const int id;
	int width;
	int height;
	int rowstride;
	bool hasAlpha;
	int bitsPerSample;
	int channels;
	QByteArray image;
};
Q_DECLARE_METATYPE(iiibiiay)

const int iiibiiay::id(qDBusRegisterMetaType<iiibiiay>());

QDBusArgument &operator<<(QDBusArgument &a, const iiibiiay &i)
{
	a.beginStructure();
	a << i.width << i.height << i.rowstride << i.hasAlpha << i.bitsPerSample << i.channels << i.image;
	a.endStructure();
	return a;
}
const QDBusArgument & operator >>(const QDBusArgument &a,  iiibiiay &i)
{
	a.beginStructure();
	a >> i.width >> i.height >> i.rowstride >> i.hasAlpha >> i.bitsPerSample >> i.channels >> i.image;
	a.endStructure();
	return a;
}

static QDBusMessage createMessage(const QString& method)
{
	return QDBusMessage::createMethodCall("org.freedesktop.Notifications",
					     "/org/freedesktop/Notifications",
					    "org.freedesktop.Notifications",
					    method);
}

DBusNotifier::DBusNotifier()
: caps_(QStringList())
{
}

DBusNotifier::~DBusNotifier()
{}

void DBusNotifier::doPopup(const QString &title, const QString &message, QImage image)
{
	QVariantMap hints;
	QString summary = title;
	if(!image.isNull()) {
		QImage im = image.scaledToWidth(defaultSize.width());
		iiibiiay i(&im);
		hints.insert("image-data", QVariant(iiibiiay::id, &i));
	}
	int lifeTime = minLifeTime;
	/*if (capabilities().contains(markupCaps)) {
		summary = TITLE_BEGIN + summary + TITLE_END;
	}*/
	QVariantList args;
	args << QString(APPLICATION_NAME);
	args << QVariant(QVariant::UInt);
	args << QVariant("");
	args << QString(summary);
	args << QString(message);
	args << QStringList();
	args << hints;
	args << lifeTime;
	doCall(args);
}

void DBusNotifier::doCall(const QVariantList &args)
{
	if(!args.isEmpty()) {
		QDBusMessage m = createMessage("Notify");
		m.setArguments(args);
		QDBusConnection::sessionBus().call(m, QDBus::NoBlock);
	}
}

bool DBusNotifier::isAvailable()
{
	return checkServer();
}

bool DBusNotifier::checkServer()
{
	QDBusInterface i("org.freedesktop.Notifications",
			"/org/freedesktop/Notifications",
			"org.freedesktop.Notifications",
			QDBusConnection::sessionBus());
	if(!i.isValid())
		return false;

	//We'll need caps in the future
	QDBusMessage m = createMessage("GetCapabilities");
	QDBusMessage ret = QDBusConnection::sessionBus().call(m);
	if(ret.type() != QDBusMessage::InvalidMessage && !ret.arguments().isEmpty()) {
		QVariant v = ret.arguments().first();
		if(v.type() == QVariant::StringList)
			caps_ = v.toStringList();
	}
	return true;
}

const QStringList &DBusNotifier::capabilities() const
{
	return caps_;
}
