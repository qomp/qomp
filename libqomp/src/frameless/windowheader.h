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


#ifndef WINDOWHEADER_H
#define WINDOWHEADER_H

#include "ui_windowheader.h"

#include <QRect>

class QMouseEvent;
class QEvent;

class WindowHeader : public QWidget
{
	Q_OBJECT
public:
	explicit WindowHeader(QWidget* p);
	~WindowHeader();

	void setCaption(const QString& caption);

private slots:
	void hidePressed();
	void closePressed();
	void maximizePressed();
        void updateIcons();
        
protected:
	void mouseDoubleClickEvent(QMouseEvent *e);
	bool eventFilter(QObject *watched, QEvent *event);

private:
	Ui::WindowHeader _ui;
};

#endif // WINDOWHEADER_H
