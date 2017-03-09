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


template <class BaseClass>
class LIBQOMPSHARED_EXPORT  AdvancedWidget : public BaseClass
{
public:
	AdvancedWidget(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());

	virtual ~AdvancedWidget();

//	void setWindowIcon(const QIcon& icon);
//	void setWindowTitle( const QString &c );
	void setUseBorder(bool isDecorated);
	bool isUseBorder();

protected:
//	void changeEvent(QEvent *event);
//	void setWindowFlags(Qt::WindowFlags flags);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseEnterEvent(const int mouse_x, const int mouse_y, const QRect &geom);
//	void moveEvent( QMoveEvent *e );
//#ifdef Q_OS_WIN
//	bool winEvent(MSG* msg, long* result);
//#endif
	void showEvent(QShowEvent *event);


private:
#ifdef Q_OS_WIN
	Qt::WindowFlags deltaflags;
#endif
	QPoint movepath;
	bool isResize;
	bool isDrag;
	bool border;
	bool showed;
	Qt::WindowFrameSection region;
};


#endif
