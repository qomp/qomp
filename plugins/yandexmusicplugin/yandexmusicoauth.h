#ifndef YANDEXMUSICOAUTH_H
#define YANDEXMUSICOAUTH_H

#include <QObject>
#include <QVariant>

class QNetworkAccessManager;
class QOAuth2AuthorizationCodeFlow;

class YandexMusicOauth : public QObject
{
	Q_OBJECT
public:
	explicit YandexMusicOauth(QObject *parent = nullptr);
	virtual ~YandexMusicOauth();
	QString userName();
	QString userId();
	QString userUid();


	static QString token();

public slots:
	void grant();

signals:
	void granted();

private:
	void startAuth();
	void openAuthUrl();
	void checkDevId();
	void setupFlow();
	void setupHandler();
	void updateToken();
	QJsonObject getUserInfo();

private:
	QNetworkAccessManager *_nam;
	QOAuth2AuthorizationCodeFlow *_flow;
	QVariant _devId;

};

#endif // YANDEXMUSICOAUTH_H
