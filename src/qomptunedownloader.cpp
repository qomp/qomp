/*
 * Copyright (C) 2013-2016  Khryukin Evgeny
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

#include "qomptunedownloader.h"
#include "qompnetworkingfactory.h"
#include "tune.h"
#include "gettuneurlhelper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QFile>
#include <QQueue>
#ifdef QOMP_MOBILE
#include <QAndroidJniObject>
#else
#include <QProgressDialog>
#include <QMessageBox>
#endif


QompTuneDownloader * QompTuneDownloader::_instance = nullptr;

class QompTuneDownloader::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QompTuneDownloader* p) : QObject(p),
		nam_(QompNetworkingFactory::instance()->getMainNAM()),
		file_(nullptr),
		reply_(nullptr)
	{
#ifndef QOMP_MOBILE
		dialog_ = new QProgressDialog();
		dialog_->setWindowTitle(tr("Download Progress"));
		dialog_->setRange(0, 100);
		connect(dialog_, &QProgressDialog::canceled, this, &Private::abort);
		dialog_->resize(dialog_->width() * 1.5, dialog_->height());
#endif
	}

	~Private()
	{
#ifdef QOMP_MOBILE
		QAndroidJniObject str = QAndroidJniObject::fromString(tr("Download finished"));
		QAndroidJniObject::callStaticMethod<void>("net/sourceforge/qomp/Qomp",
								"showNotification",
								"(Ljava/lang/String;)V",
								str.object<jstring>());
#else
		dialog_->accept();
		dialog_->deleteLater();
#endif
	}

	void start()
	{
#ifdef QOMP_MOBILE
		QAndroidJniObject str = QAndroidJniObject::fromString(tr("Download started"));
		QAndroidJniObject::callStaticMethod<void>("net/sourceforge/qomp/Qomp",
								"showNotification",
								"(Ljava/lang/String;)V",
								str.object<jstring>());
#else
		dialog_->show();
#endif
	}

	void peekNext() {
		if(downloads_.isEmpty()) {
			parent()->deleteLater();
			return;
		}

		Download dwd = downloads_.head();
		GetTuneUrlHelper* helper = new GetTuneUrlHelper(this, "urlFinished", this);
		helper->getTuneUrlAsynchronously(dwd.first);
	}

public slots:
	void downloadProgress(qint64 received, qint64 total)
	{
#ifdef QOMP_MOBILE
		Q_UNUSED(received)
		Q_UNUSED(total)
#else
		dialog_->setLabelText(tr("Total bytes:%1\nDownloaded bytes:%2")
				      .arg(QString::number(total), QString::number(received)));
		dialog_->setValue(received*100/total);
#endif
	}

	void urlFinished(const QUrl& url)
	{
		if(downloads_.isEmpty())
			return;

		Download dwd = downloads_.dequeue();

		if(url.isEmpty()) {
			finished();
			return;
		}

		Tune* t = dwd.first;

		file_ = new QFile(QString("%1/%2-%3.mp3").arg(dwd.second, t->artist, t->title));
		if(!file_->open(QFile::WriteOnly)) {
#ifdef QOMP_MOBILE
			QAndroidJniObject str = QAndroidJniObject::fromString(tr("Cann't create file!"));
			QAndroidJniObject::callStaticMethod<void>("net/sourceforge/qomp/Qomp",
									"showNotification",
									"(Ljava/lang/String;)V",
									str.object<jstring>());
#else
			QMessageBox::warning(0, tr("Error"), tr("Cann't create file!"));
#endif
			delete file_;
			file_ = nullptr;

			return;
		}

		QNetworkRequest nr(url);
		reply_ = nam_->get(nr);
		connect(reply_, &QNetworkReply::downloadProgress, this, &Private::downloadProgress);
		connect(reply_, &QNetworkReply::readyRead, this, &Private::readyRead);
		connect(reply_, &QNetworkReply::finished, this, &Private::finished);

		start();
	}

	void readyRead()
	{
		qint64  bytes = reply_->bytesAvailable();
		QByteArray ba = reply_->read(bytes);
		file_->write(ba);
	}

	void abort()
	{
		if(reply_) {
			reply_->disconnect();
			reply_->abort();
			reply_->deleteLater();
			reply_ = nullptr;
		}
		if(file_) {
			file_->remove();
			file_->deleteLater();
			file_ = nullptr;
		}
		downloads_.clear();
		dialog_->hide();
		peekNext();
	}

	void finished()
	{
		if(file_) {
			file_->close();
			file_->deleteLater();
			file_ = nullptr;
		}
		if(reply_) {
			reply_->deleteLater();
			reply_ = nullptr;
		}
		dialog_->hide();
		peekNext();
	}

public:
#ifndef QOMP_MOBILE
	QProgressDialog* dialog_;
#endif
	QNetworkAccessManager* nam_;
	QFile* file_;
	QNetworkReply* reply_;
	typedef QPair<Tune*, QString> Download;
	QQueue<Download> downloads_;
};

QompTuneDownloader::QompTuneDownloader(QObject *parent) :
	QObject(parent),
	d(new Private(this))
{
}

QompTuneDownloader *QompTuneDownloader::instance()
{
	if(!_instance)
		_instance = new QompTuneDownloader(qApp);

	return _instance;
}

QompTuneDownloader::~QompTuneDownloader()
{
	_instance = nullptr;
}

void QompTuneDownloader::download(Tune *tune, const QString &dir)
{
	d->downloads_.enqueue({tune, dir});
	if(d->downloads_.count() == 1)
		d->peekNext();
}

#include "qomptunedownloader.moc"
