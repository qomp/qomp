/*
 * updateschecker.cpp
 * Copyright (C) 2011-2013  Khryukin Evgeny
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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QRegExp>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopServices>

#include "updateschecker.h"
#include "options.h"
#include "defines.h"
#include "qompnetworkingfactory.h"

static const QString url = "https://raw.githubusercontent.com/qomp/qomp/master/libqomp/src/defines.h";
static const QString downloadUrl = "http://sourceforge.net/projects/qomp/files/";


UpdatesChecker::UpdatesChecker(QObject *parent)
	: QObject(parent)
{
	QNetworkAccessManager* manager = QompNetworkingFactory::instance()->getMainNAM();
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	QNetworkRequest request;
	request.setUrl(QUrl(url));

	QWidget *p = dynamic_cast<QWidget*>(parent);
	progressDialog_ = new QProgressDialog(p);
	progressDialog_->setWindowTitle(APPLICATION_NAME);
	progressDialog_->setWindowModality(Qt::WindowModal);
	progressDialog_->show();
	QNetworkReply* reply = manager->get(request);
	connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
}
UpdatesChecker::~UpdatesChecker()
{
	delete progressDialog_;
	progressDialog_ = 0;
}

void UpdatesChecker::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	progressDialog_->setMaximum(bytesTotal);
	progressDialog_->setValue(bytesReceived);
}

void UpdatesChecker::replyFinished(QNetworkReply *reply)
{
	progressDialog_->hide();
	if(reply->error() != QNetworkReply::NoError) {
		showError(tr("Error while checking for updates: %1").arg(reply->errorString()));
	}
	else {
		QString data = QString::fromUtf8(reply->readAll());
		const QRegExp re("#define APPLICATION_VERSION\\s+\"([^\"]+)\"");
		if(re.indexIn(data) != -1) {
			const QString ver = re.cap(1);
			const QString curVer = Options::instance()->getOption(OPTION_APPLICATION_VERSION).toString();
			if(ver != curVer) {
				int rez = QMessageBox::question(0, tr("New version is available"),
								tr("Do you want to go to the download page?"),
								QMessageBox::Yes | QMessageBox::No);
				if(rez == QMessageBox::Yes) {
					QDesktopServices::openUrl(QUrl(downloadUrl));
				}
			}
			else {
				QMessageBox::information(0, APPLICATION_NAME, tr("There is no updates found."), QMessageBox::Ok);
			}
		}
		else {
			showError(tr("Can't find information about version."));
		}
	}

	reply->close();
	reply->deleteLater();
	this->deleteLater();
}

void UpdatesChecker::showError(const QString &error)
{
	QMessageBox::warning(0, APPLICATION_NAME, error, QMessageBox::Ok);
}
