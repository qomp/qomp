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

#ifndef MPRISADAPTER_H
#define MPRISADAPTER_H

#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusObjectPath>
#include <QVariantMap>

class MprisController;

struct QompMetaData {
	QString title;
	QString artist;
	QString album;
	QString url;
	int trackNumber;
	int trackLength;
	QString cover;
};

class MprisAdapter : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
	Q_PROPERTY(QVariantMap Metadata READ metadata)
	Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
	Q_PROPERTY(bool CanGoNext READ canGoNext)
	Q_PROPERTY(bool CanGoPrevious READ canGoPrevious)
	Q_PROPERTY(bool CanPlay READ canPlay)
	Q_PROPERTY(bool CanPause READ canPause)
	Q_PROPERTY(bool CanSeek READ canSeek)
	Q_PROPERTY(bool CanControl READ canControl)
	Q_PROPERTY(qreal Volume READ getVolume WRITE setVolume)
	Q_PROPERTY(qlonglong Position READ getPosition)

public:
	explicit MprisAdapter(MprisController *p);

public slots:
	void Play();
	void Pause();
	void Next();
	void Previous();
	void PlayPause();
	void Stop();
	void SetPosition(const QDBusObjectPath &TrackId, qlonglong Position);

public:
	void setStatus(const QString &status);
	void setMetadata(const QompMetaData &tune);
	void updateProperties();

private:
	QVariantMap metadata() const;
	QString playbackStatus() const;
	bool canGoNext() const {return true;}
	bool canGoPrevious() const {return true;}
	bool canPlay() const {return true;}
	bool canPause() const {return true;}
	bool canSeek() const {return true;}
	bool canControl() const {return true;}
	void setVolume(const qreal &volume);
	qreal getVolume();
	qreal getPosition();

private:
	MprisController *controller_;
	QVariantMap metaDataMap_;
	QString playerStatus_;
	bool statusChanged_;
	bool metadataChanged_;
	QDBusObjectPath trackId_;
};

#endif // MPRISADAPTER_H
