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

Scaler::Scaler(QObject *parent) :
	QObject(parent)
{
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

void Scaler::recalculateMargins()
{
	setScaleMargins( qMin( scaleX(), scaleY() ) );
}



