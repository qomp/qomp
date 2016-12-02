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

#ifndef SCALER_H
#define SCALER_H

#include <QObject>

class Scaler : public QObject
{
	Q_OBJECT
	Q_PROPERTY(double scaleX READ scaleX WRITE setScaleX NOTIFY scaleXChanged)
	Q_PROPERTY(double scaleY READ scaleY WRITE setScaleY NOTIFY scaleYChanged)
	Q_PROPERTY(double scaleFont READ scaleFont WRITE setScaleFont NOTIFY scaleFontChanged)
	Q_PROPERTY(double scaleMargins READ scaleMargins WRITE setScaleMargins NOTIFY scaleMarginsChanged)
public:
	explicit Scaler(QObject *parent = nullptr);

	double scaleX() const;
	void setScaleX(double scaleX);

	double scaleY() const;
	void setScaleY(double scaleY);

	double scaleFont() const;
	void setScaleFont(double scaleFont);

	double scaleMargins() const;
	void setScaleMargins(double scaleMargins);

public slots:
	void updateScales();

signals:
	void scaleXChanged(double);
	void scaleYChanged(double);
	void scaleFontChanged(double);
	void scaleMarginsChanged(double);

private:
	void recalculateMargins();

private:
	double _scaleX = 1;
	double _scaleY = 1;
	double _scaleFont = 1;
	double _scaleMargins = 1;
};

#endif // SCALER_H
