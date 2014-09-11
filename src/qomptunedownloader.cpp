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


#include "qomptunedownloader.h"
#include "qompnetworkingfactory.h"
#include "tune.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QFile>

QompTuneDownloader::QompTuneDownloader(QObject *parent) :
	QObject(parent),
	file_(0),
	reply_(0)
{
	nam_ = QompNetworkingFactory::instance()->getMainNAM();
	dialog_ = new QProgressDialog();
	dialog_->setWindowTitle(tr("Download Progress"));
	dialog_->setRange(0, 100);
	connect(dialog_, SIGNAL(canceled()), SLOT(abort()));
}

QompTuneDownloader::~QompTuneDownloader()
{
	dialog_->accept();
	dialog_->deleteLater();
	delete file_;
}

void QompTuneDownloader::download(Tune *tune, const QString &dir)
{
	QUrl url(tune->getUrl());
	if(url.isEmpty())
		return;

	file_ = new QFile(QString("%1/%2-%3.mp3").arg(dir, tune->artist, tune->title));
	if(!file_->open(QFile::WriteOnly))
		return;

	QNetworkRequest nr(url);
	reply_ = nam_->get(nr);
	connect(reply_, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
	connect(reply_, SIGNAL(readyRead()), SLOT(readyRead()));
	connect(reply_, SIGNAL(finished()), SLOT(finished()));
	dialog_->show();
}

void QompTuneDownloader::downloadProgress(qint64 received, qint64 total)
{
	dialog_->setLabelText(tr("Total bytes:%1\nDownloaded bytes:%2")
			      .arg(QString::number(total), QString::number(received)));
	dialog_->setValue(received*100/total);
}

void QompTuneDownloader::finished()
{
	file_->close();
	reply_->deleteLater();
	deleteLater();
}

void QompTuneDownloader::readyRead()
{
	qint64 bytes =  reply_->bytesAvailable();
	QByteArray ba = reply_->read(bytes);
	file_->write(ba);
}

void QompTuneDownloader::abort()
{
	reply_->abort();
	reply_->deleteLater();
	file_->remove();
	deleteLater();
}
