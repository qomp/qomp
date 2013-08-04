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

#include "tune2fileplugin.h"
#include "ui_tunetofilesettings.h"
#include "qompoptionspage.h"
#include "options.h"

#include <QFileDialog>
#include <QTimer>
#include <QtPlugin>


#define T2FOPT_FILE "plugins.tunetofile.file"
#define T2FOPT_LAST_DIR "plugins.tunetofile.dir"

class T2FSettings : public QompOptionsPage
{
	Q_OBJECT

public:
	T2FSettings(QWidget* p = 0) : QompOptionsPage(p)
	{
		ui = new Ui::TuneToFileSettings;
		ui->setupUi(this);
		ui->tb_open->setIcon(qApp->style()->standardIcon(QStyle::SP_DirOpenIcon));
		connect(ui->tb_open, SIGNAL(clicked()), SLOT(getFileName()));

		restoreOptions();
	}

	~T2FSettings()
	{
		delete ui;
	}
	virtual QString name() const { return tr("Tune to File"); }
	virtual void retranslate()
	{
		ui->retranslateUi(this);
	}

public slots:
	virtual void applyOptions()
	{
		Options::instance()->setOption(T2FOPT_FILE, ui->le_file->text());
	}

	virtual void restoreOptions()
	{
		ui->le_file->setText(QDir::toNativeSeparators(Options::instance()->getOption(T2FOPT_FILE).toString()));
	}

private slots:
	void getFileName()
	{
		QString file = QFileDialog::getSaveFileName(this, tr("Store tune into file"), Options::instance()->getOption(T2FOPT_LAST_DIR, QDir::homePath()).toString());
		if(!file.isEmpty())
		{
			ui->le_file->setText(QDir::toNativeSeparators(file));
			Options::instance()->setOption(T2FOPT_LAST_DIR, QFileInfo(file).filePath());
		}
	}

private:
	Ui::TuneToFileSettings *ui;
};

Tune2FilePlugin::Tune2FilePlugin() : enabled_(false)
{
	QTimer::singleShot(0, this, SLOT(init()));
}

QompOptionsPage *Tune2FilePlugin::options()
{
	return new T2FSettings;
}

void Tune2FilePlugin::qompPlayerChanged(QompPlayer *player)
{
	player_ = player;
	connect(player_, SIGNAL(stateChanged(QompPlayer::State)), SLOT(playerStatusChanged(QompPlayer::State)));
}

void Tune2FilePlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;
}

void Tune2FilePlugin::unload()
{
	if(!enabled_ || file_.isEmpty())
		return;

	QFile f(file_);
	if(f.exists() && f.open(QFile::WriteOnly | QFile::Truncate)) {
		f.close();
	}
}

void Tune2FilePlugin::playerStatusChanged(QompPlayer::State state)
{
	if(!enabled_ || file_.isEmpty())
		return;

	QFile f(file_);
	if(f.open(QFile::WriteOnly | QFile::Truncate)) {
		if(state == QompPlayer::StatePlaying) {
			const Tune& t = player_->currentTune();
			QString str = QString("%1 %2").arg(t.artist, t.title);
			f.write(str.toUtf8());
		}
		f.close();
	}
}

void Tune2FilePlugin::optionsUpdate()
{
	file_ = Options::instance()->getOption(T2FOPT_FILE).toString();
}

void Tune2FilePlugin::init()
{
	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(optionsUpdate()));
	optionsUpdate();
}

#ifndef HAVE_QT5
Q_EXPORT_PLUGIN2(tunetofileplugin, Tune2FilePlugin)
#endif

#include "tune2fileplugin.moc"
