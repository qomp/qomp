#ifndef MPRISPLUGIN_H
#define MPRISPLUGIN_H

#include <QtDBus/QDBusAbstractAdaptor>
#include <QVariantMap>

class DBUSAdaptor;

class mprisplugin : public QDBusAbstractAdaptor
{
Q_OBJECT
Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
Q_PROPERTY(QVariantMap Metadata READ metadata)
Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
public:
	mprisplugin(QObject *parent = 0);
	~mprisplugin();
public:
	QVariantMap metadata() const;
	QString playbackStatus() const;
	setStatus(const QString &status);
	void setMetadata(int trackNumber, const QString &title, const QString &artist, const QString &album, const QString &url);
private:
	void sendProperties();
	QVariantMap metaData;
	QString playerStatus;
	bool statusChanged;
	bool metadataChanged;
};

#endif // MPRISPLUGIN_H
