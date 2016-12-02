/*
 * Copyright (C) 2016  Khryukin Evgeny, Vitaly Tonkacheyev
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

#ifndef ROOTADAPTER_H
#define ROOTADAPTER_H

#include <QtDBus/QDBusAbstractAdaptor>

class RootAdapter : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
	Q_PROPERTY(QStringList SupportedMimeTypes READ getMimeTypes)
	Q_PROPERTY(QString Identity READ getIdentity)
	Q_PROPERTY(QString DesktopEntry READ getDesktopEntry)
	Q_PROPERTY(bool CanQuit READ canQuit)
	Q_PROPERTY(bool CanRaise READ canRaise)
	Q_PROPERTY(bool CanSetFullscreen READ canSetFullscreen)
	Q_PROPERTY(bool HasTrackList READ  hasTrackList)

public:
	explicit RootAdapter(QObject *p);
	void setData();

private:
	QStringList getMimeTypes() const;
	QString getIdentity() const {return "Qomp";}
	QString getDesktopEntry() const {return "qomp";}
	bool canQuit() const {return true;}
	bool canRaise() const {return true;} //Can raise player window
	bool canSetFullscreen() const {return false;}
	bool hasTrackList() const {return false;}

public slots:
	void Raise();
	void Quit();

};

#endif // ROOTADAPTER_H
