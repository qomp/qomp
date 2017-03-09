/*
 * windowheader.cpp
 * Copyright (C) 2010, 2017  Khryukin Evgeny, Vitaly Tonkacheyev
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "windowheader.h"

#include <QGuiApplication>
#include <QMouseEvent>

WindowHeader::WindowHeader(QWidget *p)
	: QWidget(p)
{
	_ui.setupUi(this);
#ifdef Q_OS_MAC
	ui_.horiz->insertWidget(0, ui_.closeButton);
	ui_.horiz->insertWidget(1, ui_.hideButton);
	ui_.horiz->insertWidget(2, ui_.maximizeButton);
#endif

	_ui.closeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	_ui.hideButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMinButton));
	_ui.maximizeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMaxButton));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	connect(_ui.hideButton, SIGNAL(clicked()), SLOT(hidePressed()));
	connect(_ui.closeButton, SIGNAL(clicked()), SLOT(closePressed()));
	connect(_ui.maximizeButton, SIGNAL(clicked()), SLOT(maximizePressed()));

	parentWidget()->window()->installEventFilter(this);
}

WindowHeader::~WindowHeader()
{
}

void WindowHeader::hidePressed()
{
	parentWidget()->window()->showMinimized();
}

void WindowHeader::closePressed()
{
	parentWidget()->window()->close();
}

void WindowHeader::maximizePressed()
{

	if (parentWidget()->window()->windowState() != Qt::WindowMaximized) {
		parentWidget()->window()->showMaximized();
	}
	else {
		parentWidget()->window()->showNormal();
	}
}

void WindowHeader::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) {
		maximizePressed();
		e->accept();
	}
}

bool WindowHeader::eventFilter(QObject *watched, QEvent *event)
{
	if(watched == parentWidget()->window() && event->type() == QEvent::WindowStateChange) {
		if (parentWidget()->window()->windowState() == Qt::WindowMaximized) {
			_ui.maximizeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarNormalButton));
		}
		else {
			_ui.maximizeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMaxButton));
		}
	}

	return QWidget::eventFilter(watched, event);
}
