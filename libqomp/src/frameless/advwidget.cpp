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
#include "windowheader.h"
#include "thememanager.h"
#include "common.h"
#include "defines.h"

#include <QMouseEvent>
#include <QWidget>
#include <QBoxLayout>
#include <QMainWindow>
#include <QDialog>


static const int resizeAccuracy = 5;

#ifdef HAVE_QT6
#define globalPos() globalPosition().toPoint()
#endif


template<class BaseClass>
AdvancedWidget<BaseClass>::AdvancedWidget(QWidget *parent, Qt::WindowFlags f)
	: BaseClass(parent, f),
	  _action(WinAction::None),
	  _border(true)
{
	QObject::connect(ThemeManager::instance(), &ThemeManager::themeChanged, this, &AdvancedWidget::themeChanged);

	QObject::connect(this, &BaseClass::windowTitleChanged, [this](const QString& title){
		setCaption(title);
	});
}

template<class BaseClass>
AdvancedWidget<BaseClass>::~AdvancedWidget()
{
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::setupFinished()
{
	themeChanged();

	Qomp::logEvent(ITEM_VIEW_EVENT, {{ITEM_VIEW_EVENT_DATA, BaseClass::windowTitle()}});
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::setUseBorder(bool useBorder)
{
	if(useBorder == isUseBorder())
		return;

	Qt::WindowFlags flags = BaseClass::windowFlags();
	bool visible = BaseClass::isVisible();

	if (useBorder) {
//#ifdef Q_OS_WIN
//		flags |= Qt::WindowTitleHint;
//#endif
		flags &= ~Qt::FramelessWindowHint;
	}
	else {
//#ifdef Q_OS_WIN
//		flags &= ~Qt::WindowTitleHint;
//#endif
		flags |= Qt::FramelessWindowHint;
	}

	BaseClass::setWindowFlags(flags);

	_border = useBorder;
	updateHeaderState();

	BaseClass::setVisible(visible);
}

template<class BaseClass>
bool AdvancedWidget<BaseClass>::isUseBorder() const
{
	return _border;
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::setCaption(const QString &title)
{
	WindowHeader *wh = getWindowHeader();
	if (wh) {
		wh->setCaption(title);
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::themeChanged()
{
	setUseBorder(ThemeManager::instance()->isWindowBorderEnabled());
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mousePressEvent(QMouseEvent *event)
{
	if (!_border && (event->button() == Qt::LeftButton)) {
		QWidget *window = BaseClass::window();

		_region = getMouseRegion(event->globalPos().x(), event->globalPos().y(), window->geometry());
		if (_region != Qt::NoSection) {
			_action = WinAction::Resizing;
		}
		else {
			_movePath = event->globalPos() - window->pos();
			_action = WinAction::Dragging;
		}
	}

	BaseClass::mousePressEvent(event);
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mouseMoveEvent(QMouseEvent *event)
{
	if(!isUseBorder()) {
		bool isLeftButton = (event->buttons() & Qt::LeftButton);
		const QPoint pg = event->globalPos();
		QWidget *window = BaseClass::window();

		if(!isLeftButton) {
			Qt::WindowFrameSection region = getMouseRegion(pg.x(), pg.y(), window->geometry());

			updateCursor(region, window);
		}
		else if (isLeftButton && _action == WinAction::Resizing) {
			doWindowResize(window, pg, _region);
		}
		else if(isLeftButton && _action == WinAction::Dragging && !window->isMaximized()) {
			window->setCursor(QCursor(Qt::SizeAllCursor));
			window->move(pg - _movePath);
		}
	}

	BaseClass::mouseMoveEvent(event);
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::mouseReleaseEvent(QMouseEvent *event)
{
	if (!_border && (event->button() == Qt::LeftButton) && _action == WinAction::Dragging) {
		QWidget *window = BaseClass::window();

		_movePath = QPoint(0,0);
		_action = WinAction::None;
		window->setCursor(QCursor(Qt::ArrowCursor));
	}

	BaseClass::mouseReleaseEvent(event);
}

template<class BaseClass>
Qt::WindowFrameSection AdvancedWidget<BaseClass>::getMouseRegion(const int mouse_x, const int mouse_y, const QRect &geom) const
{
	const int top = geom.top();
	const int bottom = geom.bottom();
	const int left = geom.left();
	const int right = geom.right();
	const int maxtop = top + resizeAccuracy;
	const int minbottom = bottom - resizeAccuracy;

	if(qAbs(bottom - mouse_y) < resizeAccuracy
		&& qAbs(mouse_x - left) < resizeAccuracy)
	{
		return Qt::BottomLeftSection;
	}
	else if (mouse_x > (left + resizeAccuracy)
		 && mouse_x < (right - resizeAccuracy)
		 && qAbs(mouse_y - bottom) < resizeAccuracy)
	{
		return Qt::BottomSection;
	}
	else if (qAbs(bottom - mouse_y) < resizeAccuracy
		     && qAbs(mouse_x - right) < resizeAccuracy)
	{
		return Qt::BottomRightSection;
	}
	else if (qAbs(right - mouse_x) < resizeAccuracy
		 &&  mouse_y > maxtop
		 && mouse_y < minbottom)
	{
		return Qt::RightSection;
	}
	else if (qAbs(mouse_x - left) < resizeAccuracy
		 &&  mouse_y > maxtop
		 && mouse_y < minbottom)
	{
		return Qt::LeftSection;
	}
	else if (qAbs(mouse_y - top) < resizeAccuracy
		 && mouse_x > (left + resizeAccuracy)
		 && mouse_x < (right -resizeAccuracy))
	{
		return Qt::TopSection;
	}
	else if (qAbs(top - mouse_y) < resizeAccuracy
		 && qAbs(mouse_x - right) < resizeAccuracy)
	{
		return Qt::TopRightSection;
	}
	else if (qAbs(top - mouse_y) < resizeAccuracy
		 && qAbs(mouse_x - left) < resizeAccuracy)
	{
		return Qt::TopLeftSection;
	}

	return Qt::NoSection;
}

template<class BaseClass>
QBoxLayout *AdvancedWidget<BaseClass>::getMainLayout() const
{
	QBoxLayout* bl = qobject_cast<QBoxLayout*>(BaseClass::layout());
	if(!bl) {
		const QMainWindow* mw = qobject_cast<const QMainWindow*>(this);
		if (mw)
			bl = qobject_cast<QBoxLayout*>(mw->centralWidget()->layout());
	}

	return bl;
}

template<class BaseClass>
WindowHeader *AdvancedWidget<BaseClass>::getWindowHeader() const
{
	WindowHeader *wh = nullptr;

	QBoxLayout* bl = getMainLayout();
	if(bl) {
		wh = qobject_cast<WindowHeader*>(bl->itemAt(0)->widget());
	}

	return wh;
}

static void setMousetTrackingForWidgetWithChildren(QWidget* widget, bool enabled)
{
	widget->setMouseTracking(enabled);
	for(QWidget* w: widget->findChildren<QWidget*>()) {
		w->setMouseTracking(enabled);
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::updateHeaderState()
{
	QBoxLayout* bl = getMainLayout();

	if(!bl) {
		return;
	}

	if(isUseBorder()) {
		WindowHeader *wh = getWindowHeader();
		if (wh) {
			bl->removeWidget(wh);
			wh->deleteLater();
		}

		setMousetTrackingForWidgetWithChildren(this, false);

	}
	else {
		WindowHeader* wh = new WindowHeader(this);
		wh->setCaption(BaseClass::windowTitle());
		bl->insertWidget(0, wh);

		setMousetTrackingForWidgetWithChildren(this, true);
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::updateCursor(Qt::WindowFrameSection region, QWidget *window)
{
	if(window->isMaximized()) {
		window->setCursor(QCursor(Qt::ArrowCursor));
		return;
	}

	switch (region) {
	case Qt::BottomLeftSection:
		window->setCursor(QCursor(Qt::SizeBDiagCursor));
		break;
	case Qt::BottomRightSection:
		window->setCursor(QCursor(Qt::SizeFDiagCursor));
		break;
	case Qt::TopLeftSection:
		window->setCursor(QCursor(Qt::SizeFDiagCursor));
		break;
	case Qt::TopRightSection:
		window->setCursor(QCursor(Qt::SizeBDiagCursor));
		break;
	case Qt::BottomSection:
		window->setCursor(QCursor(Qt::SizeVerCursor));
		break;
	case Qt::RightSection:
		window->setCursor(QCursor(Qt::SizeHorCursor));
		break;
	case Qt::LeftSection:
		window->setCursor(QCursor(Qt::SizeHorCursor));
		break;
	case Qt::TopSection:
		window->setCursor(QCursor(Qt::SizeVerCursor));
		break;
	case Qt::NoSection:
	default:
		window->setCursor(QCursor(Qt::ArrowCursor));
		break;
	}
}

template<class BaseClass>
void AdvancedWidget<BaseClass>::doWindowResize(QWidget *window, const QPoint &eventPos, Qt::WindowFrameSection region)
{
	if(window->isMaximized())
		return;

	int ypath = 0;
	int xpath = 0;
	const QRect winGeom = window->geometry();
	const int right = winGeom.right();
	const int left =  winGeom.left();
	const int top =  winGeom.top();
	const int bottom = winGeom.bottom();

	switch (region) {
	case Qt::BottomLeftSection:
		ypath =  eventPos.y() - bottom;
		xpath = left - eventPos.x();
		if ((window->width() + xpath) < window->minimumWidth()) {
			xpath = window->minimumWidth() - window->width();
		}
		window->setGeometry(window->x() - xpath, window->y(),
				    window->width() + xpath, window->height() + ypath);
		break;
	case Qt::BottomRightSection:
		ypath = eventPos.y() - bottom;
		xpath = eventPos.x() - right;
		window->resize(window->width() + xpath, window->height() + ypath);
		break;
	case Qt::TopLeftSection:
		ypath =  top - eventPos.y();
		xpath = left - eventPos.x();
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
		ypath =  top - eventPos.y();
		xpath = eventPos.x() - right;
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
		ypath =  eventPos.y() - bottom;
		window->resize(window->width(), window->height() + ypath);
		break;
	case Qt::RightSection:
		xpath =  eventPos.x() - right;
		window->resize(window->width() + xpath, window->height());
		break;
	case Qt::LeftSection:
		xpath =  left - eventPos.x();
		if ((window->width() + xpath) < window->minimumWidth()) {
			xpath = window->minimumWidth() - window->width();
		}
		window->setGeometry(window->x() - xpath, window->y(),
				    window->width() + xpath, window->height());
		break;
	case Qt::TopSection:
		ypath =  top - eventPos.y();
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


// explicit instantiations
// we will only be able to use AdvancedWidget with next types
template class AdvancedWidget<QMainWindow>;
template class AdvancedWidget<QDialog>;
