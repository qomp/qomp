/*
 * Copyright (C) 2017  Khryukin Evgeny
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

#include "cuteradioplugingettunesdialog.h"
#include "cuteradioplugindefines.h"
#include "options.h"
#include "qompplugintreeview.h"
#include "ui_cuteradioplugingettunesdialog.h"

#include <QKeyEvent>


class CuteRadioPluginGetTunesDialog::Private :public QObject
{
	Q_OBJECT
public:
	Private() :
		_ui(new Ui::CuteRadioPluginGetTunesDialog),
		_widget(new QWidget)
	{
		_ui->setupUi(_widget);

		_ui->cb_countries->setHeaderHidden(false);
		_ui->cb_genres->setHeaderHidden(false);
		_ui->cb_countries->setHorizontalHeaderLabels({tr("Country"),tr("Count")});
		_ui->cb_genres->setHorizontalHeaderLabels({tr("Genre"),tr("Count")});

		connect(_ui->cb_countries, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
			this, &Private::countryChanged);
		connect(_ui->cb_genres, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
			this, &Private::genreChanged);
	}

	Ui::CuteRadioPluginGetTunesDialog* _ui;
	QWidget* _widget;

public slots:
	void countryChanged(const QString& country)
	{
		Options::instance()->setOption(OPTION_CUTERADIO_COUNTRY, country);
	}

	void genreChanged(const QString& genre)
	{
		Options::instance()->setOption(OPTION_CUTERADIO_GENRE, genre);
	}
};




CuteRadioPluginGetTunesDialog::CuteRadioPluginGetTunesDialog(QObject *parent) :
	QompPluginGettunesDlg(parent)
{
	p = new Private;

	setWindowTitle(CUTERADIO_PLUGIN_NAME);
	setResultsWidget(p->_widget);

	connect(p->_ui->treeView, SIGNAL(itemActivated(QModelIndex)), SLOT(itemSelected(QModelIndex)));
}

CuteRadioPluginGetTunesDialog::~CuteRadioPluginGetTunesDialog()
{
	delete p;
}

void CuteRadioPluginGetTunesDialog::setModel(QAbstractItemModel *model)
{
	p->_ui->treeView->setModel(model);
}

QString CuteRadioPluginGetTunesDialog::country() const
{
	return p->_ui->cb_countries->currentText();
}

QString CuteRadioPluginGetTunesDialog::genre() const
{
	return p->_ui->cb_genres->currentText();
}

void CuteRadioPluginGetTunesDialog::setCountries(DataPairs *items)
{
	QSignalBlocker b(p->_ui->cb_countries);
	const QString opt = Options::instance()->getOption(OPTION_CUTERADIO_COUNTRY).toString();
	for(const auto& item: *items) {
		if(p->_ui->cb_countries->findText(item.first) < 0) {
			p->_ui->cb_countries->addItem({item.first, item.second});
		}
	}

	int index = p->_ui->cb_countries->findText(opt);
	p->_ui->cb_countries->setCurrentIndex(index);
}

void CuteRadioPluginGetTunesDialog::setGenres(DataPairs *items)
{
	QSignalBlocker b(p->_ui->cb_genres);
	const QString opt = Options::instance()->getOption(OPTION_CUTERADIO_GENRE).toString();
	for(const auto& item: *items) {
		if(p->_ui->cb_genres->findText(item.first) < 0) {
			p->_ui->cb_genres->addItem({item.first, item.second});

		}
	}
	int index = p->_ui->cb_genres->findText(opt);
	p->_ui->cb_genres->setCurrentIndex(index);
}

#include "cuteradioplugingettunesdialog.moc"
