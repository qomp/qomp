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

#include "lastfmplugin.h"
#include "lastfmsettings.h"
#include "qompplayer.h"
#include "options.h"
#include "common.h"
#include "qompnetworkingfactory.h"
#include "lastfmauthdlg.h"
#include "tune.h"

#include <QtPlugin>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QTime>
#include <QDomDocument>

//#include <QtDebug>

static const QString ApiKey = "5484e1e0998b54b74fcbb81ca372c7c7";
static const QString SharedSecret = "8de4a45a27afd166dec73b9fc458ed3a";
static const QString ApiUrl = "http://ws.audioscrobbler.com/2.0/";

static QDomElement documentElemet(QNetworkReply* r)
{
	QDomDocument doc;
	QDomElement ret;
	QByteArray repl = r->readAll();
//	qDebug() << repl;
	if(doc.setContent(repl)) {
		ret = doc.documentElement();
	}
	return ret;
}

static QString MD5(const QString str)
{
	QByteArray arr = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Md5).toHex();
	return QString::fromLatin1(arr).rightJustified(32, '0').toLower();
}


LastFmPlugin::LastFmPlugin() :
	scrobbleTimer_(new QTimer(this)),
	nowPlayingTimer_(new QTimer(this)),
	enabled_(false)
{
	scrobbleTimer_->setSingleShot(true);
	connect(scrobbleTimer_, SIGNAL(timeout()), SLOT(scrobble()));

	nowPlayingTimer_->setSingleShot(true);
	connect(nowPlayingTimer_, SIGNAL(timeout()), SLOT(updateNowPlaying()));

	QTimer::singleShot(100, this, SLOT(init()));
}

void LastFmPlugin::init()
{
	nam_ = QompNetworkingFactory::instance()->getNetworkAccessManager();
}

QompOptionsPage *LastFmPlugin::options()
{
	if(!enabled_ )
		return 0;

	settings_ = new LastFmSettings;
	connect(settings_, SIGNAL(doLogin()), SLOT(login()));
	return settings_;
}

void LastFmPlugin::qompPlayerChanged(QompPlayer *player)
{
	player_ = player;
	connect(player_, SIGNAL(tuneChanged(Tune*)), SLOT(tuneChanged(Tune*)));
	connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStatusChanged()));
}

void LastFmPlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void LastFmPlugin::unload()
{
	if(settings_)
		delete settings_;
}

void LastFmPlugin::playerStatusChanged()
{
	if(!enabled_)
		return;

	scrobbleTimer_->stop();
	nowPlayingTimer_->stop();
	if(player_->state() == Qomp::StatePlaying) {
		nowPlayingTimer_->start(3000);
	}
}

void LastFmPlugin::postFinished()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
//	qDebug() << reply->readAll();
}

void LastFmPlugin::updateNowPlaying()
{
	QString sk = Options::instance()->getOption(LASTFM_SESS_KEY).toString();
	if(sk.isEmpty())
		return;

	sk = Qomp::decodePassword(sk, LASTFM_KEY);
	QTime time = QTime::fromString(currentTune_->duration, "mm:ss");
	QString dur = QString::number(time.minute()*60 + time.second());	
	const QString api_sig = MD5(QString("api_key%1artist%2duration%6methodtrack.updatenowplayingsk%3track%4%5")
				 .arg(ApiKey, currentTune_->artist, sk, currentTune_->title, SharedSecret, dur));
	QByteArray data = QString("method=track.updatenowplaying&artist=%1&track=%2&duration=%6&api_key=%3&api_sig=%4&sk=%5")
				.arg(currentTune_->artist, currentTune_->title, ApiKey, api_sig, sk, dur).toUtf8();
	QNetworkRequest nr(ApiUrl);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QNetworkReply* r = nam_->post(nr, data);
	connect(r, SIGNAL(finished()), SLOT(postFinished()));
	int t = (dur.toInt() <= 0) ? 4*60 : qMin(dur.toInt()/2, 4*60);
	scrobbleTimer_->setInterval(t*1000);
	scrobbleTimer_->start();
}

void LastFmPlugin::scrobble()
{
	QTime time = QTime::fromString(currentTune_->duration, "mm:ss");
	QString dur = QString::number(time.minute()*60 + time.second());
	QString timestamp = QString::number(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch()/1000 - scrobbleTimer_->interval()/1000);
	const QString sk = Qomp::decodePassword(Options::instance()->getOption(LASTFM_SESS_KEY).toString(), LASTFM_KEY);
	const QString api_sig = MD5(QString("api_key%1artist%2chosenByUser1duration%6methodtrack.scrobblesk%3timestamp%7track%4%5")
				 .arg(ApiKey, currentTune_->artist, sk, currentTune_->title, SharedSecret, dur, timestamp));
	QByteArray data = QString("method=track.scrobble&chosenByUser=1&artist=%1&track=%2&duration=%6&timestamp=%7&api_key=%3&api_sig=%4&sk=%5")
				.arg(currentTune_->artist, currentTune_->title, ApiKey, api_sig, sk, dur, timestamp).toUtf8();
	QNetworkRequest nr(ApiUrl);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QNetworkReply* r = nam_->post(nr, data);
	connect(r, SIGNAL(finished()), SLOT(postFinished()));
}

void LastFmPlugin::tuneChanged(Tune *t)
{
	currentTune_ = t;
}

void LastFmPlugin::login()
{
	QString api_sig = MD5(QString("api_key%1methodauth.getToken%2").arg(ApiKey, SharedSecret));
	QString	url = QString("%1?method=auth.gettoken&api_key=%2&api_sig=%3").arg(ApiUrl,ApiKey, api_sig);
	QNetworkRequest nr(url);
	QNetworkReply* r = nam_->get(nr);
	connect(r, SIGNAL(finished()), SLOT(loginStepTwo()));
}

void LastFmPlugin::loginStepTwo()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();

	if(reply->error() == QNetworkReply::NoError) {
		QDomElement elem = documentElemet(reply);
		const QString token = elem.firstChildElement("token").text();
		if(!token.isEmpty()) {
			LastFmAuthDlg dlg;
			if(dlg.openUrl(QString("http://www.last.fm/api/auth/?api_key=%1&token=%2").arg(ApiKey, token)) == LastFmAuthDlg::Accepted) {
				QString api_sig = MD5(QString("api_key%1methodauth.getsessiontoken%2%3").
						      arg(ApiKey, token, SharedSecret));
				QString	url = QString("%1?method=auth.getsession&api_key=%2&token=%3&api_sig=%4")
						.arg(ApiUrl, ApiKey, token, api_sig);
				QNetworkRequest nr(url);
				QNetworkReply* r = nam_->get(nr);
				connect(r, SIGNAL(finished()), SLOT(loginStepThree()));
			}
		}
	}
}

void LastFmPlugin::loginStepThree()
{
	QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QDomElement elem = documentElemet(reply);
		const QString key = elem.firstChildElement("session").firstChildElement("key").text();
		Options::instance()->setOption(LASTFM_SESS_KEY, Qomp::encodePassword(key, LASTFM_KEY));
		const QString name = elem.firstChildElement("session").firstChildElement("name").text();
		Options::instance()->setOption(LASTFM_OPT_USER, name);
		if(settings_) {
			settings_->applyOptions();
			settings_->restoreOptions();
		}
	}
}




#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(lastfmplugin, LastFmPlugin)
#endif
