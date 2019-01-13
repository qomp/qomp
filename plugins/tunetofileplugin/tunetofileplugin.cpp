/*
 * Copyright (C) 2013-2015  Khryukin Evgeny
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

#include "tunetofileplugin.h"
#include "tunetofilesettings.h"
#include "options.h"
#include "tune.h"

#include <QTimer>
#include <QtPlugin>
#include <QFile>


TuneToFilePlugin::TuneToFilePlugin() : player_(0), enabled_(false)
{
	QTimer::singleShot(0, this, SLOT(init()));
}

QompOptionsPage *TuneToFilePlugin::options()
{
	if(!enabled_)
		return 0;

	optPage_ = new TuneToFileSettings;
	return optPage_;
}

void TuneToFilePlugin::qompPlayerChanged(QompPlayer *player)
{
	if(player_ != player) {
		if(player_)
			disconnect(player_, SIGNAL(stateChanged(Qomp::State)), this, SLOT(playerStatusChanged(Qomp::State)));

		player_ = player;
		if(player_)
			connect(player_, SIGNAL(stateChanged(Qomp::State)), SLOT(playerStatusChanged(Qomp::State)));
	}
}

void TuneToFilePlugin::setEnabled(bool enabled)
{
	enabled_ = enabled;

	if(!enabled && !file_.isEmpty()) {
		QFile f(file_);
		if(f.exists() && f.open(QFile::WriteOnly | QFile::Truncate)) {
			f.close();
		}
	}
}

void TuneToFilePlugin::unload()
{
	if(optPage_)
		delete optPage_;
}

void TuneToFilePlugin::playerStatusChanged(Qomp::State state)
{
	if(!enabled_ || file_.isEmpty() || !player_)
		return;

	QFile f(file_);
	if(f.open(QFile::WriteOnly | QFile::Truncate)) {
		if(state == Qomp::StatePlaying) {
			Tune* t = player_->currentTune();
			f.write(t->displayString().toUtf8());
		}
		f.close();
	}
}

void TuneToFilePlugin::optionsUpdate()
{
	file_ = Options::instance()->getOption(T2FOPT_FILE).toString();
}

void TuneToFilePlugin::init()
{
	connect(Options::instance(), SIGNAL(updateOptions()), SLOT(optionsUpdate()));
	optionsUpdate();
}
