/*
 * Copyright (C) 2013-2014  Khryukin Evgeny
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

#include "tunetofilesettings.h"
#include "options.h"

#include "ui_tunetofilesettings.h"

#include <QFileDialog>

class TuneToFileSettings::Private
{
public:
	Private(TuneToFileSettings* p) :
		page_(p),
		widget_(new QWidget),
		ui(new Ui::TuneToFileSettings)
	{
		ui->setupUi(widget_);
		ui->tb_open->setIcon(qApp->style()->standardIcon(QStyle::SP_DirOpenIcon));
		QObject::connect(ui->tb_open, SIGNAL(clicked()), page_, SLOT(getFileName()));
	}

	TuneToFileSettings* page_;
	QWidget* widget_;
	Ui::TuneToFileSettings* ui;
};



TuneToFileSettings::TuneToFileSettings(QObject *parent) :
	QompOptionsPage(parent)
{
	d = new Private(this);
	restoreOptions();
}

TuneToFileSettings::~TuneToFileSettings()
{
	delete d->ui;
	delete d;
}

QString TuneToFileSettings::name() const
{
	return tr("Tune to File");
}

void TuneToFileSettings::retranslate()
{
	d->ui->retranslateUi(d->widget_);
}

QObject *TuneToFileSettings::page() const
{
	return d->widget_;
}

void TuneToFileSettings::applyOptions()
{
	Options::instance()->setOption(T2FOPT_FILE, d->ui->le_file->text());
}

void TuneToFileSettings::restoreOptions()
{
	d->ui->le_file->setText(QDir::toNativeSeparators(Options::instance()->getOption(T2FOPT_FILE).toString()));
}

void TuneToFileSettings::getFileName()
{
	QString file = QFileDialog::getSaveFileName(d->widget_, tr("Store tune into file"),
				Options::instance()->getOption(T2FOPT_LAST_DIR, QDir::homePath()).toString());
	if(!file.isEmpty())
	{
		d->ui->le_file->setText(QDir::toNativeSeparators(file));
		Options::instance()->setOption(T2FOPT_LAST_DIR, QFileInfo(file).filePath());
	}
}
