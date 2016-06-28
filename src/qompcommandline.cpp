/*
 * Copyright (C) 2016  Khryukin Evgeny
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

#include "qompcommandline.h"
#include "defines.h"

#ifndef Q_OS_ANDROID
#include <QCommandLineParser>
#include <QApplication>
#else
#include <QAndroidJniObject>
#include <QtAndroid>
#endif

QompCommandLine::QompCommandLine(QObject *parent) : QObject(parent)
{
	parse();
}

const QStringList &QompCommandLine::args() const
{
	return _args;
}

void QompCommandLine::parse()
{
#ifdef Q_OS_ANDROID
	QAndroidJniObject act = QtAndroid::androidActivity();
	QAndroidJniObject str = act.callObjectMethod("checkIntent", "()Ljava/lang/String;");
	if(str.isValid()) {
		_args.append(str.toString());
	}
#else
	QCommandLineParser p;
	p.setApplicationDescription(QStringLiteral(APPLICATION_NAME) + ' ' + QStringLiteral(APPLICATION_VERSION));
	p.addHelpOption();
	p.addVersionOption();
	p.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
	p.addPositionalArgument("file/url", tr("The file (url) to open."));
	p.process(*qApp);

	_args = p.positionalArguments();
#endif
}
