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

#include "qompplayer.h"

#include <QMediaPlayer>
#include <QFutureWatcher>
#include <QPointer>

class QompMetaDataResolver;
class QAudioOutputSelectorControl;

class QompQtMultimediaPlayer : public QompPlayer
{
	Q_OBJECT
public:
	QompQtMultimediaPlayer();
	~QompQtMultimediaPlayer();

	virtual void setVolume(qreal vol) Q_DECL_OVERRIDE;
	virtual qreal volume() const Q_DECL_OVERRIDE;
	virtual void setMute(bool mute) Q_DECL_OVERRIDE;
	virtual bool isMuted() const Q_DECL_OVERRIDE;
	virtual void setPosition(qint64 pos) Q_DECL_OVERRIDE;
	virtual qint64 position() const Q_DECL_OVERRIDE;

	virtual Qomp::State state() const Q_DECL_OVERRIDE;
	virtual void play() Q_DECL_OVERRIDE;
	virtual void pause() Q_DECL_OVERRIDE;
	virtual void stop() Q_DECL_OVERRIDE;
	virtual qint64 currentTuneTotalTime() const Q_DECL_OVERRIDE;

	virtual QStringList audioOutputDevice() const Q_DECL_OVERRIDE;
	virtual void setAudioOutputDevice(const QString& devName) Q_DECL_OVERRIDE;
	virtual QString defaultAudioDevice() const Q_DECL_OVERRIDE;

protected slots:
	virtual void doSetTune() Q_DECL_OVERRIDE;

private slots:
	void volumeChanged(int);
	void playerStateChanged(QMediaPlayer::PlaybackState);
	void mediaStatusChanged(QMediaPlayer::MediaStatus status);
	void tuneUrlReady(const QUrl& url);
	void tuneDurationChanged(qint64 dur);
	void tunePositionChanged(qint64 pos);
	void audioReadyChanged(bool ready);
	void seekableChanged(bool seekable);

protected:
	virtual QompMetaDataResolver* metaDataResolver() const Q_DECL_OVERRIDE;

private:
	/**
	 * @brief mapPositionForTune map tune position to whole track length
	 */
	qint64 mapPositionFromTune(qint64 pos) const;
	/**
	 * @brief mapPositionFromTune map whole track position to tune position
	 */
	qint64 mapPositionFromTrack(qint64 pos) const;
	void updatePlayerPosition();
	void setPlayerMediaContent(const QUrl &url);
	bool isTuneChangeFinished() const;
	void processMediaState(bool audioReady, bool seekable);
	void resumePlayer();
	bool isMediaReady(bool audioReady, bool seekable) const;
	void doPlay();

private:
	QMediaPlayer* player_;
	QompMetaDataResolver* resolver_;
	QPointer< QFutureWatcher<QUrl> > watcher_;
	QPointer<const Tune> prevTune_;
};

#endif // QOMPQTMULTIMEDIAPLAYER_H
