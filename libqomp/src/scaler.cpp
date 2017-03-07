/*
 * Copyright (C) 2014  Khryukin Evgeny
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

#include "scaler.h"

#include <QGuiApplication>
#include <QRect>
#include <QScreen>

#ifdef DEBUG_OUTPUT
#include <QDebug>
#endif

Scaler::Scaler(QObject *parent) :
	QObject(parent)
{
	updateScales();
}

double Scaler::scaleX() const
{
	return _scaleX;
}

void Scaler::setScaleX(double scaleX)
{
	if(scaleX != _scaleX) {
		_scaleX = scaleX;
		recalculateMargins();
		emit scaleXChanged(_scaleX);
	}
}
double Scaler::scaleY() const
{
	return _scaleY;
}

void Scaler::setScaleY(double scaleY)
{
	if(scaleY != _scaleY) {
		_scaleY = scaleY;
		recalculateMargins();
		emit scaleXChanged(_scaleY);
	}
}

double Scaler::scaleFont() const
{
	return _scaleFont;
}

void Scaler::setScaleFont(double scaleFont)
{
	if(scaleFont != _scaleFont) {
		_scaleFont = scaleFont;
		emit scaleFontChanged(_scaleFont);
	}
}

double Scaler::scaleMargins() const
{
	return _scaleMargins;
}

void Scaler::setScaleMargins(double scaleMargins)
{
	if(_scaleMargins != scaleMargins) {
		_scaleMargins = scaleMargins;
		emit scaleMarginsChanged(_scaleMargins);
	}
}

void Scaler::updateScales()
{
#ifdef Q_OS_ANDROID
	QSizeF refPhysSize(49, 80);
	qreal refDpi = 108.;
	qreal refHeight = 800.;
	qreal refWidth = 480.;

	QRect geometry = qApp->primaryScreen()->geometry();
	qreal height = qMax(geometry.width(), geometry.height());
	qreal width = qMin(geometry.width(), geometry.height());

	qreal dpi = qApp->primaryScreen()->logicalDotsPerInch();
	qreal dpr = qApp->primaryScreen()->devicePixelRatio();

	QSizeF physSize = qApp->primaryScreen()->physicalSize();

	Q_UNUSED(refDpi)
	Q_UNUSED(dpi)
	Q_UNUSED(dpr)

#ifdef DEBUG_OUTPUT
	qDebug() << "dpi:" << dpi;
	qDebug() << "dpr:" << dpr;
	qDebug() << "geometry:" << geometry;
	qDebug() << "physSize:" << physSize;
#endif

	setScaleX((width / physSize.width()) / (refWidth / refPhysSize.width()));
	setScaleY((height / physSize.height()) / (refHeight / refPhysSize.height()));

//	qreal physHeight = qMax(physSize.width(), physSize.height());
//	qreal physWidth = qMin(physSize.width(), physSize.height());
//	qreal factorX = pow( (physWidth / refPhysSize.width()), 1.0 / 3.0);
//	qreal factorY = pow( (physHeight / refPhysSize.height()), 1.0 / 3.0);

//	setScaleX(width*refDpi/(dpi*refWidth) * factorX * dpr);
//	setScaleY(height*refDpi/(dpi*refHeight) * factorY * dpr);

	recalculateMargins();
#endif
}

void Scaler::recalculateMargins()
{
#ifdef Q_OS_ANDROID
	setScaleMargins( qMin( scaleX(), scaleY() ) );
	setScaleFont( scaleMargins() );
#endif
}
