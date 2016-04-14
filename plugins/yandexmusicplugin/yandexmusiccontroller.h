/*
 * Copyright (C) 2014-2016  Khryukin Evgeny
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

class QompPluginTreeModel;
class QNetworkReply;
class YandexMusicGettunsDlg;
class QNetworkRequest;
class QUrl;
class QPixmap;

class YandexMusicController : public QompPluginController
{
	Q_OBJECT
public:
	explicit YandexMusicController(QObject *parent = 0);
	virtual ~YandexMusicController();

protected slots:
	void doSearch(const QString& text);
	QompPluginGettunesDlg* view() const;
	void itemSelected(QompPluginModelItem* item);
	void getSuggestions(const QString& text);
	void suggestionsFinished();

protected:
	void init();
	QList<Tune*> prepareTunes() const;

private slots:
	void artistsSearchFinished();
	void albumsSearchFinished();
	void tracksSearchFinished();
	void albumUrlFinished();
	void artistUrlFinished();
	void makeQuery();

signals:
	void queryFinished();

private:
	void checkAndStopBusyWidget();
	void search(const QString& text, const QString& type, const char* slot, int page = 0);
	void searchNextPage(const QByteArray &reply, const QString& type, const char* slot);
	QNetworkRequest creatNetworkRequest(const QUrl& url) const;
	bool checkRedirect(QNetworkReply* reply, const char *slot, QompPluginTreeModel* model = nullptr);
	bool checkCaptcha(const QUrl& url, const QByteArray& reply, const char *slot, QompPluginTreeModel* model = nullptr);
	QPixmap getCaptcha(const QString& captchaUrl, QString* key);

private:
	QompPluginTreeModel *tracksModel_, *albumsModel_, *artistsModel_;
	QHash<QNetworkReply*, void*> requests_;
	YandexMusicGettunsDlg* dlg_;
	QString mainUrl_;
	QHash<QString, const char*> queries_;
	QString searchText_;
};

#endif // YANDEXMUSICCONTROLLER_H
