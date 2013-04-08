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
#include "qompplugintreemodel.h"
#include "qompplugintypes.h"
#include "options.h"
#include "prostopleerplugindefines.h"
#include "common.h"

#include "ui_prostopleerpluginresultswidget.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMessageBox>
#include <QKeyEvent>
#include <QNetworkProxy>


class ProstopleerTune : public QompPluginTune
{
public:
	ProstopleerTune() : QompPluginTune() {}
	QString duration;
	QString durationToStr() const
	{
		return durationSecondsToString(duration.toInt());
	}

	virtual Tune toTune() const
	{
		Tune t = QompPluginTune::toTune();
		t.duration = durationToStr();
		return t;
	}
};

ProstoPleerPluginGetTunesDialog::ProstoPleerPluginGetTunesDialog(QWidget *parent) :
	QompPluginGettunesDlg(parent),
	ui(new Ui::ProstoPleerPluginResultsWidget),
	model_(new QompPluginTreeModel(this))
{
	setWindowTitle(PROSTOPLEER_PLUGIN_NAME);

	QWidget* widget = new QWidget(this);
	ui->setupUi(widget);
	setResultsWidget(widget);

	ui->tv_results->setModel(model_);
	ui->tb_prev->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
	ui->tb_next->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));

	ui->tb_prev->hide();

	connect(ui->tv_results, SIGNAL(clicked(QModelIndex)), SLOT(itemSelected(QModelIndex)));
	connect(ui->tb_next, SIGNAL(clicked()), SLOT(actNextActivated()));
	connect(ui->tb_prev, SIGNAL(clicked()), SLOT(actPrevActivated()));

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
	foreach(QompPluginModelItem* item, model_->selectedItems()) {
		if(!item)
			continue;

		ProstopleerTune *pt = static_cast<ProstopleerTune*>(item);
		if(!pt->url.isNull()) {
			tunes_.append(pt->toTune());
		}
	}

	QompPluginGettunesDlg::accept();
}


void ProstoPleerPluginGetTunesDialog::doSearch()
{
	model_->reset();
	QString text = currentSearchText();
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
		QList<QompPluginModelItem*> list;
		QString result = reply->readAll();
		int off = 0;
		while((off = re.indexIn(result, off)) != -1) {
			off += re.matchedLength();
			ProstopleerTune* tune = new ProstopleerTune();
			tune->artist = QString::fromUtf8(re.cap(3).toLatin1());
			tune->title = QString::fromUtf8(re.cap(4).toLatin1());
			tune->internalId = re.cap(5);
			tune->duration = re.cap(1);
			list.append(tune);
		}
		if(!list.isEmpty())
			model_->addTopLevelItems(list);

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

void ProstoPleerPluginGetTunesDialog::actPrevActivated()
{
}

void ProstoPleerPluginGetTunesDialog::actNextActivated()
{
	doSearchStepTwo();
}

void ProstoPleerPluginGetTunesDialog::itemSelected(const QModelIndex &index)
{
	QompPluginModelItem* item = model_->item(index);
	if(!item)
		return;

	ProstopleerTune* pt = static_cast<ProstopleerTune*>(item);
	if(!pt->url.isEmpty())
		return;

	QUrl url("http://prostopleer.com");
	url.setPath("/site_api/files/get_url");
	QNetworkRequest nr(url);
	nr.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	nr.setRawHeader("Accept", "application/json, text/javascript");
	nr.setRawHeader("X-Requested-With", "XMLHttpRequest");
	QByteArray ba("action=play&id=");
	ba += pt->internalId;
	QNetworkReply *reply = nam_->post(nr, ba);
	reply->setProperty("id", pt->internalId);
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
			ProstopleerTune* pt = static_cast<ProstopleerTune*>(model_->itemForId(id));
			pt->url = re.cap();
			model_->emitUpdateSignal();
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
