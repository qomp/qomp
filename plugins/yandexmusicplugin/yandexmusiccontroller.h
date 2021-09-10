/*
 * Copyright (C) 2014-2021  Khryukin Evgeny
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

#ifndef YANDEXMUSICCONTROLLER_H
#define YANDEXMUSICCONTROLLER_H

#include "qompplugincontroller.h"
#include <QHash>
#include <QUrl>

class QompPluginTreeModel;
class QNetworkReply;
class YandexMusicGettunsDlg;
class QNetworkRequest;
class QNetworkReply;
class QUrl;
class QPixmap;
class YandexMusicCaptcha;

class YandexMusicController : public QompPluginController
{
	Q_OBJECT
public:
	explicit YandexMusicController(QObject *parent = nullptr);
	~YandexMusicController();

protected slots:
	void doSearch(const QString& txt) Q_DECL_FINAL;
	QompPluginGettunesDlg* view() const Q_DECL_FINAL;
	void itemSelected(QompPluginModelItem* item) Q_DECL_FINAL;
	void getSuggestions(const QString& text) Q_DECL_FINAL;
	void suggestionsFinished() Q_DECL_FINAL;

protected:
	void init() Q_DECL_FINAL;
	QList<Tune*> prepareTunes() const Q_DECL_FINAL;

private slots:
	void artistsSearchFinished();
	void albumsSearchFinished();
	void tracksSearchFinished();
	void albumUrlFinished();
	void artistUrlFinished();
	void makeQuery(int num);
	void processCaptha(QNetworkReply* r);

signals:
	void queryFinished();

private:
	void checkAndStopBusyWidget();
	void search(const QString& text, const QString& type, const char* slot, int page = 0);
	bool searchNextPage(const QByteArray &reply, const QString& type, const char* slot);
	QNetworkRequest creatNetworkRequest(const QUrl& url) const;
	bool checkCaptcha(const QUrl& replyUrl, const QByteArray& reply, const char *slot, QompPluginTreeModel* model = nullptr);

private:
	QompPluginTreeModel *tracksModel_, *albumsModel_, *artistsModel_;
	QHash<QNetworkReply*, void*> requests_;
	YandexMusicGettunsDlg* dlg_;
	QString mainUrl_;

	typedef QPair<QString, const char*> Query;
	QHash<int, Query> queries_;
	QString searchText_;

	struct PendingRequst {
		PendingRequst(const QUrl& u, const char* s, QompPluginTreeModel* m) :
			url(u), slot(s), model(m)
		{}

		QUrl url;
		const char* slot;
		QompPluginTreeModel* model;
	};

	QList<PendingRequst> pendingRequests_;
	QHash<int, bool> searched_;
	YandexMusicCaptcha *captcha_;
};

#endif // YANDEXMUSICCONTROLLER_H
