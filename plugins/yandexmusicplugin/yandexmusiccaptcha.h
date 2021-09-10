#ifndef YANDEXMUSICCAPTCHA_H
#define YANDEXMUSICCAPTCHA_H

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class YandexMusicCaptcha : public QObject
{
	Q_OBJECT
public:
	explicit YandexMusicCaptcha(QNetworkAccessManager *nam, QObject *parent = nullptr);

	enum CheckResult {InProgress, NoCaptcha, CaptchaSuccess, Resolving};

	CheckResult checkCaptcha(const QUrl& replyUrl, const QByteArray& reply);
	QPixmap getCaptcha(const QString& captchaUrl, QString* key);

signals:
	void captchaReady(QNetworkReply* r);
private:
	QNetworkAccessManager *nam_;
	bool captchaInProgress_;

};

#endif // YANDEXMUSICCAPTCHA_H
