/*
 * updateschecker.cpp
 * Copyright (C) 2011-2014  Khryukin Evgeny
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


class UpdatesChecker::Private : public QObject
{
	Q_OBJECT
public:
	Private(UpdatesChecker* p) :
		QObject(p),
		progressDialog_(0),
		interactive_(true)
	{
	}

	~Private()
	{
		delete progressDialog_;
	}

	void start()
	{
		QNetworkAccessManager* manager = QompNetworkingFactory::instance()->getMainNAM();
		connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
		QNetworkRequest request;
		request.setUrl(QUrl(url));

		if(interactive_) {
			progressDialog_ = new QProgressDialog();
			progressDialog_->setWindowTitle(APPLICATION_NAME);
			progressDialog_->setWindowModality(Qt::ApplicationModal);
			progressDialog_->show();
		}

		QNetworkReply* reply = manager->get(request);
		if(interactive_)
			connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
	}

	void showError(const QString& error)
	{
		if(interactive_)
			QMessageBox::warning(0, APPLICATION_NAME, error, QMessageBox::Ok);
	}

	void showInfo(const QString& info)
	{
		if(interactive_)
			QMessageBox::information(0, APPLICATION_NAME, info, QMessageBox::Ok);
	}

	UpdatesChecker* uc() const
	{
		return static_cast<UpdatesChecker*>(parent());
	}

public slots:
	void replyFinished(QNetworkReply* reply)
	{
		if(progressDialog_)
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
					uc()->hasUpdate_ = true;
					if(interactive_) {
						int rez = QMessageBox::question(0, tr("New version is available"),
									tr("Do you want to go to the download page?"),
									QMessageBox::Yes | QMessageBox::No);
						if(rez == QMessageBox::Yes) {
							QDesktopServices::openUrl(QUrl(downloadUrl));
						}
					}
				}
				else {
					showInfo(tr("There is no updates found."));
				}
			}
			else {
				showError(tr("Can't find information about version."));
			}
		}

		reply->close();
		reply->deleteLater();
		deleteLater();
	}

	void updateProgress(qint64 bytesReceived, qint64 bytesTotal)
	{
		if(progressDialog_) {
			progressDialog_->setMaximum(bytesTotal);
			progressDialog_->setValue(bytesReceived);
		}
	}

public:
	QProgressDialog* progressDialog_;
	bool interactive_;
};



UpdatesChecker::UpdatesChecker(QObject *parent)
	: QObject(parent),
	  d(new Private(this)),
	  hasUpdate_(false)
{	
}

UpdatesChecker::~UpdatesChecker()
{
}

void UpdatesChecker::startCheck(bool interactive)
{
	d->interactive_ = interactive;
	d->start();
}

bool UpdatesChecker::hasUpdate() const
{
	return hasUpdate_;
}

#include "updateschecker.moc"
