/*
 * advwidget.cpp - AdvancedWidget template class
 * Copyright (C) 2017 Khryukin Evgeny
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


#include "advwidget.h"
#include <windowheader.h>

#include <QMouseEvent>
#include <QWidget>
#include <QBoxLayout>
#include <QMainWindow>
#include <QDialog>


template<class BaseClass>
AdvancedWidget<BaseClass>::AdvancedWidget(QWidget *parent, Qt::WindowFlags f)
	: BaseClass(parent, f),
	  border(true)
{
}

template<class BaseClass>
AdvancedWidget<BaseClass>::~AdvancedWidget()
{

}

template<class BaseClass>
void AdvancedWidget<BaseClass>::setUseBorder(bool isDecorated)
{
	if(isDecorated == isUseBorder())
		return;

	Qt::WindowFlags flags = BaseClass::windowFlags();
	bool visible = BaseClass::isVisible();
#ifdef Q_OS_WIN
	if (deltaflags == 0) {
		deltaflags = flags;
	}
	if (isDecorated) {
		if (flags != deltaflags) {
			flags |= Qt::WindowTitleHint;
			flags &= ~Qt::FramelessWindowHint;
			deltaflags = 0;
			if (flags != BaseClass::windowFlags()) {
				BaseClass::setWindowFlags(flags);
			}
		}
	} else {
		flags &= ~Qt::WindowTitleHint;
		flags |= Qt::FramelessWindowHint;
		if (flags != BaseClass::windowFlags()) {
			BaseClass::setWindowFlags(flags);
		}

	}
#else
	if (isDecorated) {
		flags &= ~Qt::FramelessWindowHint;
	} else {
		flags |= Qt::FramelessWindowHint;
	}
	if (flags != BaseClass::windowFlags()) {
		BaseClass::setWindowFlags(flags);
	}
#endif
	border = isDecorated;

	updateHeaderState();
	BaseClass::setVisible(visible);
}

template<class BaseClass>
bool AdvancedWidget<BaseClass>::isUseBorder()
{
	return border;
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mousePressEvent(QMouseEvent *event)
{
	QWidget *window = BaseClass::window();
	if (!border && (event->button()==Qt::LeftButton)) {
		mouseEnterEvent(event->globalPos().x(), event->globalPos().y(), window->geometry());
		if (region != Qt::NoSection) {
			isResize = true;
		}
		else{
			movepath = event->globalPos() - window->pos();
			isResize = false;
		}
		isDrag = true;
		BaseClass::mousePressEvent(event);
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mouseMoveEvent(QMouseEvent *event)
{
	bool isLeftButton = (event->buttons() & Qt::LeftButton);
	const QPoint pg = event->globalPos();
	QWidget *window = BaseClass::window();
	int ypath = 0;
	int xpath = 0;
	const int right = window->geometry().right();
	const int left =  window->geometry().left();
	const int top =  window->geometry().top();
	const int bottom = window->geometry().bottom();
	if (isLeftButton && isResize && !border) {
		switch (region) {
		case Qt::BottomLeftSection:
			window->setCursor(QCursor(Qt::SizeBDiagCursor));
			ypath =  pg.y() - bottom;
			xpath = left - pg.x();
			if ((window->width() + xpath) < window->minimumWidth()) {
				xpath = window->minimumWidth() - window->width();
			}
			window->setGeometry(window->x() - xpath, window->y(),
					    window->width() + xpath, window->height() + ypath);
			break;
		case Qt::BottomRightSection:
			window->setCursor(QCursor(Qt::SizeFDiagCursor));
			ypath = pg.y() - bottom;
			xpath = pg.x() - right;
			window->resize(window->width() + xpath, window->height() + ypath);
			break;
		case Qt::TopLeftSection:
			window->setCursor(QCursor(Qt::SizeFDiagCursor));
			ypath =  top - pg.y();
			xpath = left - pg.x();
			if ((window->width() + xpath) < window->minimumWidth()) {
				xpath = window->minimumWidth() - window->width();
			}
			if ((window->height() + ypath) < window->minimumHeight()) {
				ypath = window->minimumHeight() - window->height();
			}
			window->setGeometry(window->x() - xpath, window->y() - ypath,
					    window->width() + xpath, window->height() + ypath);
			break;
		case Qt::TopRightSection:
			window->setCursor(QCursor(Qt::SizeBDiagCursor));
			ypath =  top - pg.y();
			xpath = pg.x() - right;
			if ((window->width() + xpath) < window->minimumWidth()) {
				xpath = window->minimumWidth() - window->width();
			}
			if ((window->height() + ypath) < window->minimumHeight()) {
				ypath = window->minimumHeight() - window->height();
			}
			window->setGeometry(window->x(), window->y() - ypath,
					    window->width() + xpath, window->height() + ypath);
			break;
		case Qt::BottomSection:
			window->setCursor(QCursor(Qt::SizeVerCursor));
			ypath =  pg.y() - bottom;
			window->resize(window->width(), window->height() + ypath);
			break;
		case Qt::RightSection:
			window->setCursor(QCursor(Qt::SizeHorCursor));
			xpath =  pg.x() - right;
			window->resize(window->width() + xpath, window->height());
			break;
		case Qt::LeftSection:
			window->setCursor(QCursor(Qt::SizeHorCursor));
			xpath =  left - pg.x();
			if ((window->width() + xpath) < window->minimumWidth()) {
				xpath = window->minimumWidth() - window->width();
			}
			window->setGeometry(window->x() - xpath, window->y(),
					    window->width() + xpath, window->height());
			break;
		case Qt::TopSection:
			window->setCursor(QCursor(Qt::SizeVerCursor));
			ypath =  top - pg.y();
			if ((window->height() + ypath) < window->minimumHeight()) {
				ypath = window->minimumHeight() - window->height();
			}
			window->setGeometry(window->x(), window->y() - ypath,
					    window->width(), window->height() + ypath);
			break;
		case Qt::NoSection:
		default:
			break;
		}
	}
	else if(isLeftButton && isDrag && !isResize && !border) {
		window->setCursor(QCursor(Qt::ArrowCursor));
		window->move( event->globalPos() - movepath );
	}
	BaseClass::mouseMoveEvent(event);
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mouseReleaseEvent(QMouseEvent *event)
{
	QWidget *window = BaseClass::window();
	if (!border && (event->button() == Qt::LeftButton) && isDrag) {
		movepath = QPoint(0,0);
		isDrag = false;
		isResize = false;
		window->setCursor(QCursor(Qt::ArrowCursor));
		BaseClass::mouseReleaseEvent(event);
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mouseEnterEvent(const int mouse_x, const int mouse_y, const QRect &geom)
{
	const int top = geom.top();
	const int bottom = geom.bottom();
	const int left = geom.left();
	const int right = geom.right();
	const int delta = 10;
	const int maxtop = top + delta;
	const int minbottom = bottom -delta;
	if(mouse_y <= bottom
			&& mouse_y >= minbottom
			&& qAbs(mouse_x - left) < delta) {
		region = Qt::BottomLeftSection;
	}
	else if (mouse_x > (left + delta)
		 && mouse_x < (right - delta)
		 && qAbs(mouse_y - bottom) < delta) {
		region = Qt::BottomSection;
	}
	else if ((bottom - mouse_y) < delta
		     && qAbs(mouse_x - right) < delta){
		region = Qt::BottomRightSection;
	}
	else if ((right - mouse_x) < delta
		 &&  mouse_y > maxtop
		 && mouse_y < minbottom) {
		region = Qt::RightSection;
	}
	else if ((mouse_x - left) < delta
		 &&  mouse_y > maxtop
		 && mouse_y < minbottom) {
		region = Qt::LeftSection;
	}
	else if ((mouse_y - top) < delta
		 && mouse_x > (left + delta)
		 && mouse_x < (right -delta)){
		region = Qt::TopSection;
	}
	else if ((top - mouse_y) < delta
		 && qAbs(mouse_x - right) < delta){

		region = Qt::TopRightSection;
	}
	else if ((top - mouse_y) < delta
		 && qAbs(mouse_x - left) < delta){
		region = Qt::TopLeftSection;
	}
	else {
		region = Qt::NoSection;
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::updateHeaderState()
{
	QBoxLayout* bl = qobject_cast<QBoxLayout*>(BaseClass::layout());
	if(!bl) {
		QMainWindow* mw = qobject_cast<QMainWindow*>(this);
		if (mw)
			bl = qobject_cast<QBoxLayout*>(mw->centralWidget()->layout());
	}

	if(!bl) {
		return;
	}

	if(isUseBorder()) {
		WindowHeader *wh = qobject_cast<WindowHeader*>(bl->itemAt(0)->widget());
		if (wh) {
			bl->removeWidget(wh);
			wh->deleteLater();
		}

	}
	else {
		bl->insertWidget(0, new WindowHeader(this));
	}
}


// explicit instantiations
// we will only be able to use AdvancedWidget with next types
template class AdvancedWidget<QMainWindow>;
template class AdvancedWidget<QDialog>;
