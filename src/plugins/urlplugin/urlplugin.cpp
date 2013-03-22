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

#include "urlplugin.h"

#include <QInputDialog>
#include <QtPlugin>

UrlPlugin::UrlPlugin()
{
}

TuneList UrlPlugin::getTunes()
{
	TuneList list;
	bool ok = false;
	QString url = QInputDialog::getText(0, tr("Input url"), "URL:",QLineEdit::Normal, "", &ok);
	if(ok) {
		Tune tune;
		tune.url = url;
		list.append(tune);
	}

	return list;
}

Q_EXPORT_PLUGIN2(urlplugin, UrlPlugin)
