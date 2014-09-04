/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "yandexmusicgettunsdlg.h"

#include "qompqmlengine.h"
#include <QQuickItem>
#include <QAbstractItemModel>

class YandexMusicGettunsDlg::Private : public QObject
{
	Q_OBJECT
public:
	Private(YandexMusicGettunsDlg* p) :
		QObject(p)

	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/YandexMusicResultsView.qml"));
		connect(item_, SIGNAL(itemCheckClick(QJSValue)), SLOT(clicked(QJSValue)));
		connect(this, SIGNAL(itemClicked(QModelIndex)), p, SLOT(itemSelected(QModelIndex)));

		p->setResultsWidget(item_);
	}
signals:
	void itemClicked(const QModelIndex&);

private slots:
	void clicked(const QJSValue& val)
	{
		QModelIndex i = val.toVariant().value<QModelIndex>();
		emit itemClicked(i);
	}

public:
	QQuickItem* item_;
};

YandexMusicGettunsDlg::YandexMusicGettunsDlg() :
	QompPluginGettunesDlg()
{
	setWindowTitle("Yandex.Music");
	p = new Private(this);
}

YandexMusicGettunsDlg::~YandexMusicGettunsDlg()
{
	delete p;
}

void YandexMusicGettunsDlg::setCuuretnTab(YandexMusicTabKind kind)
{
	p->item_->setProperty("currentTab", kind);
}

void YandexMusicGettunsDlg::setModel(QAbstractItemModel *model, YandexMusicTabKind kind)
{
	switch (kind) {
	case TabAlbum:
		p->item_->setProperty("albumsModel", QVariant::fromValue(model));
		break;
	case TabArtist:
		p->item_->setProperty("artistsModel", QVariant::fromValue(model));
		break;
	case TabTrack:
		p->item_->setProperty("tracksModel", QVariant::fromValue(model));
		break;
	}
}

int YandexMusicGettunsDlg::currentTabRows() const
{
	return static_cast<QAbstractItemModel*>(p->item_->property("currentModel").value<QObject*>())->rowCount();
}

#include "yandexmusicgettunsdlg_mobile.moc"
