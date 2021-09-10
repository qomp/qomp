#include "yandexmusicoauth.h"
#include "options.h"
#include "qompnetworkingfactory.h"

#include <QDesktopServices>
#include <QUrlQuery>
#include <QUrl>
#include <QCoreApplication>
#include <QUuid>
//#include <QTcpServer>
//#include <QTcpSocket>

#include <QNetworkRequest>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#define CLIENT_ID "23cabbbdc6cd418abb4b39c32c41195d"
#define CLIENT_SECRET "53bc75238f0c4d08a118e51fe9203300"

static const QString  optionYaDeviceId = "plugins.yandex_music.device_id";
static const QString  optionYaToken = "plugins.yandex_music.ya_token";
static const QString  optionYaTokenTtl = "plugins.yandex_music.ya_token_ttl";
static const QString  optionYaUserInfo = "plugins.yandex_music.user_info";

static const QString qompID = "371640f2734e43a4a0ab949c607a1a01";
static const QString qompPass = "68d3f871e4444d3babf9ff2629dbe115";
static const QString qompCallback = "yandexta://qomp.sourceforge.io";
static const QString qompTokenUrl = "https://oauth.yandex.ru/token";
static const QString qompAuthUrl = "https://oauth.yandex.ru/authorize";
static const QString qompAuthScope = "login:email";
//static const QString redirectUri = "http://127.0.0.1:1337";
static const qint16 port = 1337;


YandexMusicOauth::YandexMusicOauth(QObject *parent) : QObject(parent),
	_nam(QompNetworkingFactory::instance()->getThreadedNAM()),
	_flow(new QOAuth2AuthorizationCodeFlow(_nam, this)),
	_devId(Options::instance()->getOption(optionYaDeviceId))
{
	checkDevId();
	setupFlow();
}

YandexMusicOauth::~YandexMusicOauth()
{
	delete _nam;
}

QString YandexMusicOauth::userName()
{
	QJsonObject jo = getUserInfo();
	return jo.value("login").toString();
}

QString YandexMusicOauth::userId()
{
	QJsonObject jo = getUserInfo();
	return jo.value("id").toString();
}

QString YandexMusicOauth::userUid()
{
	QJsonObject jo = getUserInfo();
	return jo.value("psuid").toString();
}

QString YandexMusicOauth::token()
{
	return Options::instance()->getOption(optionYaToken).toString();
}

void YandexMusicOauth::grant()
{
//	startAuth();
	_flow->grant();
}

void YandexMusicOauth::startAuth()
{
//	QTcpServer *serv = new QTcpServer(this);
//	connect (serv, &QTcpServer::newConnection, [serv]() {
//		QTcpSocket *s = serv->nextPendingConnection();
//		connect(s, &QTcpSocket::readyRead, [=]() {
//			while(s->canReadLine())
//				qDebug() << s->readLine();
//			qDebug() << s->readAll();
//			s->write("HTTP/1.1 200 OK");
//			s->write("\r\nOK");
//			s->flush();
//			s->close();
//			s->deleteLater();
//			serv->close();
//			serv->deleteLater();
//		});
//	});

//	if(serv->listen(QHostAddress::LocalHost, port)) {
//		openAuthUrl();
//	}
}

void YandexMusicOauth::openAuthUrl()
{
	QUrl url(qompAuthUrl);
	QUrlQuery q(url);
	q.addQueryItem("client_id", qompID);
	q.addQueryItem("device_id", _devId.toString());
	q.addQueryItem("device_name", QCoreApplication::applicationName());
//	q.addQueryItem("client_secret", qompPass);

	q.addQueryItem("scope", qompAuthScope);
	q.addQueryItem("response_type", "token");

	q.addQueryItem("display", "popup");
	q.addQueryItem("force_confirm", "yes");
	q.addQueryItem("state", "token");

//	q.addQueryItem("redirect_uri", redirectUri);
//	q.addQueryItem("login_hint", "token");
//	q.addQueryItem("optional_scope", "token");

	url.setQuery(q);
	QDesktopServices::openUrl(url);
}

void YandexMusicOauth::checkDevId()
{
	if(!_devId.isValid()) {
		_devId = QUuid::createUuid().toString();
		Options::instance()->setOption(optionYaDeviceId, _devId);
	}
}

void YandexMusicOauth::setupFlow()
{
	_flow->setAuthorizationUrl(qompAuthUrl);
	_flow->setAccessTokenUrl(qompTokenUrl);
	_flow->setClientIdentifier(qompID);
	_flow->setClientIdentifierSharedKey(qompPass);
	_flow->setScope(qompAuthScope);

	connect(_flow, &QOAuth2AuthorizationCodeFlow::statusChanged, [=](QAbstractOAuth::Status status) {
		if (status == QAbstractOAuth::Status::Granted) {
			updateToken();
			emit granted();
		}
	});

	_flow->setModifyParametersFunction([=](QAbstractOAuth::Stage stage, QVariantMap *parameters) {
		if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
			parameters->insert("device_id", _devId);
			parameters->insert("device_name", QCoreApplication::applicationName());
			parameters->insert("display", "popup");
			parameters->insert("force_confirm", "yes");
//			parameters->remove("redirect_uri");
		}
	});

	connect(_flow, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);

	setupHandler();
}

void YandexMusicOauth::setupHandler()
{
	auto replyHandler = new QOAuthHttpServerReplyHandler(QHostAddress::LocalHost, port, this);
	connect(replyHandler, &QOAuthHttpServerReplyHandler::callbackReceived, [=](const QVariantMap &values) {
		foreach (auto val, values) {
			qDebug() << val;
		}
	});
	_flow->setReplyHandler(replyHandler);
	//	connect(_flow, &QOAuth2AuthorizationCodeFlow::granted, this, &YandexMusicOauth::granted);
}

void YandexMusicOauth::updateToken()
{
	Options::instance()->setOption(optionYaToken, _flow->token());
}

QJsonObject YandexMusicOauth::getUserInfo()
{
	QJsonObject jo;
	QVariant info = Options::instance()->getOption(optionYaUserInfo);
	if(!info.isValid()) {
		QEventLoop l;
		QUrl url("https://login.yandex.ru/info?format=json");
		QNetworkRequest r(url);
		const QString token = YandexMusicOauth::token();
		if(token.size() > 0) {
			r.setRawHeader("Authorization", QString("OAuth %1").arg(token).toLatin1());
			QNetworkReply *reply = _nam->get(r);
			connect(reply, &QNetworkReply::finished, [&]() {
				reply->deleteLater();
				if(reply->error() == QNetworkReply::NoError) {
					info = reply->readAll();
					Options::instance()->setOption(optionYaUserInfo, info);
				}
				l.exit();
			});
			l.exec();
		}
	}
	QJsonDocument doc = QJsonDocument::fromJson(info.toByteArray());
	jo = doc.object();
	return jo;
}
