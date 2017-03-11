/*
 * advwidget.h - AdvancedWidget template class
 * Copyright (C) 2005-2007  Michail Pishchagin, 2017 Khryukin Evgeny
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef ADVWIDGET_H
#define ADVWIDGET_H

#include <QPoint>
#include "libqomp_global.h"

class QShowEvent;
class QMouseEvent;
class QRect;
class QWidget;
class QBoxLayout;
class WindowHeader;


template <class BaseClass>
class LIBQOMPSHARED_EXPORT  AdvancedWidget : public BaseClass
{
public:
	AdvancedWidget(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

	virtual ~AdvancedWidget();

	void setUseBorder(bool isDecorated);
	bool isUseBorder() const;

protected:
	virtual void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	enum class WinAction{None, Dragging, Resizing};

	void updateHeaderState();
	void updateCursor(Qt::WindowFrameSection region, QWidget* window);
	void doWindowResize(QWidget* window, const QPoint& eventPos, Qt::WindowFrameSection region);
	Qt::WindowFrameSection getMouseRegion(const int mouse_x, const int mouse_y, const QRect &geom) const;
	QBoxLayout* getMainLayout() const;
	WindowHeader* getWindowHeader() const;
	void setCaption(const QString& title);

private:
	QPoint _movePath;
	WinAction _action;
	bool _border;
	Qt::WindowFrameSection _region;
};

#endif
