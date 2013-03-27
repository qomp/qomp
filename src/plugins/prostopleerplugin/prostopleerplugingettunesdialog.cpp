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

#include "prostopleerplugingettunesdialog.h"
#include "prostopleermodel.h"
#include "options.h"
#include "prostopleerpluginsettingsdlg.h"
#include "prostopleerplugindefines.h"
#include "common.h"
#include "qompnetworkingfactory.h"

#include "ui_prostopleerplugingettunesdialog.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QKeyEvent>
#include <QNetworkProxy>


ProstoPleerPluginGetTunesDialog::ProstoPleerPluginGetTunesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProstoPleerPluginGetTunesDialog),
	model_(new ProstopleerModel(this))
{
	ui->setupUi(this);
	ui->lv_results->setModel(model_);
	ui->tb_prev->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
	ui->tb_next->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));

	ui->tb_prev->hide();

	nam_ = QompNetworkingFactory::instance()->getNetworkAccessManager();

//	ui->lineEdit->installEventFilter(this);
	ui->lv_results->installEventFilter(this);

	connect(ui->pb_search, SIGNAL(clicked()), SLOT(doSearch()));
	connect(ui->lv_results, SIGNAL(pressed(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	connect(ui->lv_results, SIGNAL(activated(QModelIndex)), SLOT(itemActivated(QModelIndex)));
	connect(ui->tb_next, SIGNAL(clicked()), SLOT(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()), SLOT(actPrevActivated()));
	connect(ui->tb_settings, SIGNAL(clicked()), SLOT(doSettings()));

	doLogin();
}

ProstoPleerPluginGetTunesDialog::~ProstoPleerPluginGetTunesDialog()
{
	delete ui;
}

TuneList ProstoPleerPluginGetTunesDialog::getTunes() const
{
	return tunes_;
}

void ProstoPleerPluginGetTunesDialog::accept()
{
	for(int i = 0; i < model_->rowCount(); i++) {
		QModelIndex index = model_->index(i);
		if(index.data(Qt::CheckStateRole).toBool()){
			ProstopleerTune pt = model_->tune(index);
			if(pt.url.isNull())
				continue;

			Tune tune;
			tune.artist = pt.artist;
			tune.title = pt.title;
			tune.url = pt.url;
			tune.duration = pt.durationToStr();
			tunes_.append(tune);
		}
	}

	QDialog::accept();
}


void ProstoPleerPluginGetTunesDialog::doSearch()
{
	model_->reset();
	QString text = ui->lineEdit->text();
	if(text.isEmpty())
		return;

	lastSearchStr_ = text;
	ui->lb_current->setText("0");
	doSearchStepTwo();
}

void ProstoPleerPluginGetTunesDialog::doSearchStepTwo()
{
	QString url = QString("http://prostopleer.com/search?q=%1").arg(lastSearchStr_);
	int page = ui->lb_current->text().toInt();

	if(page > 0) {
		url += QString("&page=%1").arg(QString::number(++page));
	}
	QNetworkRequest nr(url);
	QNetworkReply *reply = nam_->get(nr);
	connect(reply, SIGNAL(finished()), SLOT(searchFinished()));
}

void ProstoPleerPluginGetTunesDialog::searchFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QRegExp re("<li duration=\"([\\d]+)\" file_id=\"([^\"]+)\" singer=\"([^\"]+)\" "
			   "song=\"([^\"]+)\" link=\"([^\"]+)\"");
		re.setMinimal(true);
		QList<ProstopleerTune> list;
		QString result = reply->readAll();
		int off = 0;
		while((off = re.indexIn(result, off)) != -1) {
			off += re.matchedLength();
			ProstopleerTune tune;
			tune.artist = QString::fromUtf8(re.cap(3).toLatin1());
			tune.title = QString::fromUtf8(re.cap(4).toLatin1());
			tune.id = re.cap(5);
			tune.duration = re.cap(1);
			list.append(tune);
		}
		if(!list.isEmpty())
			model_->addTunes(list);

		QRegExp page("<ul class=\"pagination\" end=\"(\\d+)\"");
		if(page.indexIn(result) != -1) {
			ui->lb_pages->setText(page.cap(1));
		}
		QRegExp curPage("&page=(\\d+)");
		if(curPage.indexIn(reply->url().toString()) != -1) {
			ui->lb_current->setText(curPage.cap(1));
		}
		else {
			ui->lb_current->setText("1");
		}

		int cur = ui->lb_current->text().toInt();
		int total = ui->lb_pages->text().toInt();
		ui->tb_next->setEnabled(cur < total);
	}
	else {
		QMessageBox::critical(this, tr("Error"), reply->errorString());
	}
}

void ProstoPleerPluginGetTunesDialog::itemActivated(const QModelIndex &index)
{
	model_->setData(index, 3); //change selection state
}

void ProstoPleerPluginGetTunesDialog::actPrevActivated()
{
}

void ProstoPleerPluginGetTunesDialog::actNextActivated()
{
	doSearchStepTwo();
}

void ProstoPleerPluginGetTunesDialog::doSettings()
{
	ProstoPleerPluginSettingsDlg dlg(this);
	if(dlg.exec() == QDialog::Accepted)
		doLogin();
}

bool ProstoPleerPluginGetTunesDialog::eventFilter(QObject *o, QEvent *e)
{
	if(o == ui->lv_results) {
		if(e->type() == QEvent::KeyPress) {
			QKeyEvent* ke = static_cast<QKeyEvent*>(e);
			if(ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Space) {
				QModelIndex index = ui->lv_results->currentIndex();
				model_->setData(index, 3);
				e->accept();
				return true;
			}
		}
	}
	return QDialog::eventFilter(o, e);
}

void ProstoPleerPluginGetTunesDialog::keyPressEvent(QKeyEvent *e)
{
	if(e->key() == Qt::Key_Return && ui->lineEdit->hasFocus()) {
		doSearch();
		e->accept();
		return;
	}
	return QDialog::keyPressEvent(e);
}

void ProstoPleerPluginGetTunesDialog::itemSelected(const QModelIndex &index)
{
	if(!model_->tune(index).url.isEmpty())
		return;

	ProstopleerTune pt = model_->tune(index);
	QUrl url("http://prostopleer.com");
	url.setPath("/site_api/files/get_url");
	QNetworkRequest nr(url);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	nr.setRawHeader("Accept", "application/json, text/javascript");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QByteArray ba("action=play&id=");
	ba += pt.id;
	QNetworkReply *reply = nam_->post(nr, ba);
	reply->setProperty("id", pt.id);
	connect(reply, SIGNAL(finished()), SLOT(urlFinished()));
}

void ProstoPleerPluginGetTunesDialog::urlFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	if(reply->error() == QNetworkReply::NoError) {
		QString id = reply->property("id").toString();
		QRegExp re("http://[^\"]+");
		QString text = reply->readAll();
		if(re.indexIn(text) != -1) {
			model_->urlChanged(id, re.cap());
		}
	}
}

void ProstoPleerPluginGetTunesDialog::doLogin()
{
	QUrl url("http://prostopleer.com");
	url.setPath("/login");
	QNetworkRequest nr(url);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	nr.setRawHeader("Accept", "application/json, text/javascript");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QString str = QString("login=%1&password=%2").arg(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN).toString(),
				decodePassword(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD).toString(), PROSTOPLEER_DECODE_KEY));
	QNetworkReply *reply = nam_->post(nr, str.toLatin1());
	connect(reply, SIGNAL(finished()), SLOT(loginFinished()));
}

void ProstoPleerPluginGetTunesDialog::loginFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	reply->deleteLater();
	QRegExp re("\"success\":([^,]+)");
	QString result = reply->readAll();
	if(re.indexIn(result) != -1) {
		if(re.cap(1) == "true") {
			ui->lb_auth->setText(tr("OK"));
			return;
		}
	}
	ui->lb_auth->setText(tr("None"));
}
