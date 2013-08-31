#ifndef MPRIS_H
#define MPRIS_H

#include <QtDBus/QDBusAbstractAdaptor>
#include <QVariantMap>

//class DBUSAdaptor;

class Mpris : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")
	Q_PROPERTY(QVariantMap Metadata READ metadata)
    Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
public:
    Mpris(QObject *p = 0);
public:
	QVariantMap metadata() const;
	QString playbackStatus() const;
	void setStatus(const QString &status);
	void setMetadata(int trackNumber, const QString &title, const QString &artist, const QString &album, const QString &url);
	void sendProperties();

private:
	QVariantMap metaData;
	QString playerStatus;
	bool statusChanged;
	bool metadataChanged;
};

#endif // MPRIS_H
