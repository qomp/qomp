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

#include "myzukarugettunesdlg.h"
#include "myzukarudefines.h"


#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

#include "qompqmlengine.h"
#include <QQuickItem>
#include <QAbstractItemModel>

class MyzukaruGettunesDlg::Private : public QObject
{
	Q_OBJECT
public:
	explicit Private(MyzukaruGettunesDlg* p) :
		QObject(p),
		item_(0)
	{
		item_ = QompQmlEngine::instance()->createItem(QUrl("qrc:///qml/MyzukaRuResultsView.qml"));
		connect(item_, SIGNAL(itemCheckClick(QVariant)), SLOT(clicked(QVariant)));
		connect(this, SIGNAL(itemClicked(QModelIndex)), p, SLOT(itemSelected(QModelIndex)));

		p->setResultsWidget(item_);
	}

signals:
	void itemClicked(const QModelIndex&);

private slots:
	void clicked(const QVariant& val)
	{
		QModelIndex i = val.value<QModelIndex>();
		emit itemClicked(i);
	}

public:
	QQuickItem* item_;
};




MyzukaruGettunesDlg::MyzukaruGettunesDlg(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	p = new Private(this);
	setWindowTitle(MYZUKA_PLUGIN_NAME);
}

MyzukaruGettunesDlg::~MyzukaruGettunesDlg()
{
	delete p;
}

void MyzukaruGettunesDlg::setModel(QAbstractItemModel *model, MyzikaruTabKind kind)
{
	switch (kind) {
	case TabAlbums:
		p->item_->setProperty("albumsModel", QVariant::fromValue(model));
		break;
	case TabArtists:
		p->item_->setProperty("artistsModel", QVariant::fromValue(model));
		break;
	case TabTracks:
		p->item_->setProperty("tracksModel", QVariant::fromValue(model));
		break;
	}
}

void MyzukaruGettunesDlg::setCurrentTab(MyzikaruTabKind kind)
{
	p->item_->setProperty("currentTab", kind);
}


#include "myzukarugettunesdlg_mobile.moc"
