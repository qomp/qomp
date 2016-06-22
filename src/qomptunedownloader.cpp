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

#include "qomptunedownloader.h"
#include "qompnetworkingfactory.h"
#include "tune.h"
#include "gettuneurlhelper.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#ifdef QOMP_MOBILE
#include <QAndroidJniObject>
#else
#include <QProgressDialog>
#include <QMessageBox>
#endif


class QompTuneDownloader::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(QompTuneDownloader* p) : QObject(p),
		nam_(QompNetworkingFactory::instance()->getMainNAM()),
		file_(0),
		reply_(0),
		tune_(nullptr)
	{
#ifndef QOMP_MOBILE
		dialog_ = new QProgressDialog();
		dialog_->setWindowTitle(tr("Download Progress"));
		dialog_->setRange(0, 100);
		connect(dialog_, SIGNAL(canceled()), SLOT(abort()));
#endif
	}

	~Private()
	{
		delete file_;
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
		if(url.isEmpty()) {
			parent()->deleteLater();
			return;
		}

		file_ = new QFile(QString("%1/%2-%3.mp3").arg(dir_, tune_->artist, tune_->title));
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
			return;
		}

		QNetworkRequest nr(url);
		reply_ = nam_->get(nr);
		connect(reply_, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
		connect(reply_, SIGNAL(readyRead()), SLOT(readyRead()));
		connect(reply_, SIGNAL(finished()), SLOT(finished()));
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
		reply_->abort();
		reply_->deleteLater();
		file_->remove();
		parent()->deleteLater();
	}

	void finished()
	{
		file_->close();
		reply_->deleteLater();
		parent()->deleteLater();
	}

public:
#ifndef QOMP_MOBILE
	QProgressDialog* dialog_;
#endif
	QNetworkAccessManager* nam_;
	QFile* file_;
	QNetworkReply* reply_;
	QString dir_;
	Tune* tune_;
};

QompTuneDownloader::QompTuneDownloader(QObject *parent) :
	QObject(parent),
	d(new Private(this))
{
}

QompTuneDownloader::~QompTuneDownloader()
{
}

void QompTuneDownloader::download(Tune *tune, const QString &dir)
{
	d->dir_ = dir;
	d->tune_ = tune;

	GetTuneUrlHelper* helper = new GetTuneUrlHelper(d, "urlFinished", this);
	helper->getTuneUrlAsynchronously(tune);
}

#include "qomptunedownloader.moc"
