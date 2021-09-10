#ifndef YANDEXMUSICOAUTH_H
#define YANDEXMUSICOAUTH_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkRequest;
class YandexMusicCaptcha;

class YandexMusicOauth : public QObject
{
	Q_OBJECT
public:
	explicit YandexMusicOauth(QObject *parent = nullptr);
	virtual ~YandexMusicOauth();
	QString userName();

	static QString token();
	static QDateTime tokenTtl();

	static void setupRequest(QNetworkRequest *nr);

public slots:
	void grant(const QString& user, const QString& password);

signals:
	void granted();
	void requestError(const QString&);

private:
	void setupHandler();
	void updateToken();

private slots:
	void grantRequestFinished();

private:
	QNetworkAccessManager *_nam;
	YandexMusicCaptcha *captcha_;

};

#endif // YANDEXMUSICOAUTH_H
