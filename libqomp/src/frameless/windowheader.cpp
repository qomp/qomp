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

	bool swapLayout = false;
#ifdef Q_OS_MAC
	swapLayout = true;
#elif defined HAVE_X11
	QString desk(getenv("XDG_CURRENT_DESKTOP"));
	swapLayout = (desk.compare(QStringLiteral("Unity"), Qt::CaseInsensitive) == 0);
#endif

	if(swapLayout) {
		QBoxLayout* l = static_cast<QBoxLayout*>(_ui.headerFrame->layout());
		l->removeWidget(_ui.hideButton);
		int ind = l->indexOf(_ui.maximizeButton);
		l->insertWidget(ind+1, _ui.hideButton);
		setLayoutDirection(Qt::RightToLeft);
	}

	_ui.closeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	_ui.hideButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMinButton));
	_ui.maximizeButton->setIcon(qApp->style()->standardIcon(QStyle::SP_TitleBarMaxButton));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	connect(_ui.hideButton, SIGNAL(clicked()), SLOT(hidePressed()));
	connect(_ui.closeButton, SIGNAL(clicked()), SLOT(closePressed()));
	connect(_ui.maximizeButton, SIGNAL(clicked()), SLOT(maximizePressed()));

	if(! (parentWidget()->window()->windowFlags() & Qt::WindowMaximizeButtonHint))
		_ui.maximizeButton->hide();

	if(! (parentWidget()->window()->windowFlags() & Qt::WindowMinimizeButtonHint))
		_ui.hideButton->hide();

	parentWidget()->window()->installEventFilter(this);
}

WindowHeader::~WindowHeader()
{
}

void WindowHeader::setCaption(const QString &caption)
{
	_ui.lblCaption->setText(caption);
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
	if (e->button() == Qt::LeftButton
		&& (parentWidget()->window()->windowFlags() & Qt::WindowMaximizeButtonHint)) {
		maximizePressed();
		e->accept();
	}
	else {
		QWidget::mouseDoubleClickEvent(e);
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
