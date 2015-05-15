/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#ifndef QOMPQTMULTIMEDIAPLAYER_H
#define QOMPQTMULTIMEDIAPLAYER_H

#include "qompplayerimpl.h"

#include <QMediaPlayer>
#include <QFutureWatcher>
#include <QPointer>

class QompMetaDataResolver;

class QompQtMultimediaPlayer : public QompPlayerImpl
{
	Q_OBJECT
public:
	QompQtMultimediaPlayer();
	~QompQtMultimediaPlayer();

	Q_INVOKABLE virtual qreal volume() const;
	Q_INVOKABLE virtual bool isMuted() const;
	Q_INVOKABLE virtual qint64 position() const;

	Q_INVOKABLE virtual Qomp::State state() const;
	Q_INVOKABLE virtual qint64 currentTuneTotalTime() const;
	Q_INVOKABLE virtual QStringList audioOutputDevice() const;

public slots:
	virtual void play();
	virtual void pause();
	virtual void stop();

	virtual void setPosition(qint64 pos);
	virtual void setMute(bool mute);
	virtual void setVolume(qreal vol);

	virtual void setAudioOutputDevice(const QString& devName);

protected slots:
	virtual void doSetTune();

private slots:
	void volumeChanged(int);
	void playerStateChanged(QMediaPlayer::State);
	void mediaStatusChanged(QMediaPlayer::MediaStatus status);
	void tuneUrlReady(const QUrl& url);

protected:
	Q_INVOKABLE virtual QompMetaDataResolver* metaDataResolver() const;

private:
	QMediaPlayer* player_;
	QompMetaDataResolver* resolver_;
	QPointer< QFutureWatcher<QUrl> > watcher_;
};

#endif // QOMPQTMULTIMEDIAPLAYER_H
