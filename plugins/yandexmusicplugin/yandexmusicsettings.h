/*
 * Copyright (C) 2021  Khryukin Evgeny
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

#ifndef LASTFMSETTINGS_H
#define LASTFMSETTINGS_H

#include "qompoptionspage.h"

class YandexMusicOauth;

class YandexMusicSettings : public QompOptionsPage
{
	Q_OBJECT
	
public:
	explicit YandexMusicSettings(QObject *parent = nullptr);
	~YandexMusicSettings();
	virtual QString name() const Q_DECL_FINAL;
	virtual void retranslate() Q_DECL_FINAL;
	virtual QObject* page() const Q_DECL_FINAL;

public slots:
	virtual void applyOptions() Q_DECL_FINAL;
	virtual void restoreOptions() Q_DECL_FINAL;

signals:
	void authRequest(const QString&, const QString&);

private slots:
	void doAuth();
	
private:
	YandexMusicOauth *_auth;
	class Private;
	Private* d;
};

#endif // LASTFMSETTINGS_H
