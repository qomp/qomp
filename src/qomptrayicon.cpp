/*
 * Copyright (C) 2013-2016  Khryukin Evgeny
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

#include "qomptrayicon.h"
#include "defines.h"
#include "qompmainwin.h"
#include "options.h"
#include "thememanager.h"

#include <QWheelEvent>
#include <QTimer>
#include <QApplication>
//#include <QtDebug>


class QompTrayAction : public QObject
{
	Q_OBJECT
public:
	virtual void exequte()
	{
		emit executed();
		deleteLater();
	}

signals:
	void executed();

protected:
	explicit QompTrayAction(QompMainWin* win) :
		QObject(win),
		win_(win)
	{}

	QompMainWin* win_;
};

class PlayPauseAction : public QompTrayAction
{
	Q_OBJECT
public:
	explicit PlayPauseAction(QompMainWin* win) : QompTrayAction(win)
	{
		connect(this, SIGNAL(executed()), win, SIGNAL(actPlayActivated()));
	}
};

class PlayNextAction : public QompTrayAction
{
	Q_OBJECT
public:
	explicit PlayNextAction(QompMainWin* win) : QompTrayAction(win)
	{
		connect(this, SIGNAL(executed()), win, SIGNAL(actNextActivated()));
	}
};

class PlayPrevAction : public QompTrayAction
{
	Q_OBJECT
public:
	explicit PlayPrevAction(QompMainWin* win) : QompTrayAction(win)
	{
		connect(this, SIGNAL(executed()), win, SIGNAL(actPrevActivated()));
	}
};

class ToggleVisibilityAction : public QompTrayAction
{
	Q_OBJECT
public:
	explicit ToggleVisibilityAction(QompMainWin* win) : QompTrayAction(win) {}

	void exequte()
	{
		win_->toggleVisibility();
		deleteLater();
	}
};



QompTrayIcon::QompTrayIcon(QompMainWin *parent) :
	QObject(parent),
	icon_(new QSystemTrayIcon(this)),
	win_(parent),
	lastClick_(QTime::currentTime())
{
	setIcon(ThemeManager::instance()->getIconFromTheme(":/icons/icons/qomp_stop.png"));
	setToolTip(QString());
	connect(icon_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));

	icon_->installEventFilter(this);
}

void QompTrayIcon::setToolTip(const QString &text)
{
	if(text.isEmpty())
		icon_->setToolTip(QString("%1 %2").arg(APPLICATION_NAME, APPLICATION_VERSION));
	else
		icon_->setToolTip(QString("%1 %2\n%3").arg(APPLICATION_NAME, APPLICATION_VERSION, text));
}

void QompTrayIcon::setIcon(const QIcon &ico)
{
	icon_->setIcon(ico);
	emit iconChanged(ico);
}

const QIcon QompTrayIcon::icon() const
{
	return icon_->icon();
}

void QompTrayIcon::setContextMenu(QMenu *m)
{
	icon_->setContextMenu(m);
}

void QompTrayIcon::show()
{
	icon_->show();
}

QStringList QompTrayIcon::availableActions()
{
	static const QStringList actions = QStringList() << QObject::tr("Play/Pause")
							<< QObject::tr("Toggle Visibility")
							<< QObject::tr("Play Next")
							<< QObject::tr("Play Previous");
	return actions;
}

QompTrayActionType QompTrayIcon::actionTimeForName(const QString &name)
{
	return availableActions().indexOf(name);
}

QompTrayAction *QompTrayIcon::actionForType(QompTrayActionType type) const
{
	switch(type) {
	case 0:
		return new PlayPauseAction(win_);
	case 1:
		return new ToggleVisibilityAction(win_);
	case 2:
		return new PlayNextAction(win_);
	case 3:
		return new PlayPrevAction(win_);
	}

	return 0;
}

void QompTrayIcon::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	QompTrayAction *action = 0;
	lastClick_ = QTime::currentTime();
	switch(reason) {
	case QSystemTrayIcon::DoubleClick:
		action = actionForType(Options::instance()->getOption(OPTION_TRAY_DOUBLE_CLICK).toInt());
		break;
	case QSystemTrayIcon::MiddleClick:
		action = actionForType(Options::instance()->getOption(OPTION_TRAY_MIDDLE_CLICK).toInt());
		break;
	case QSystemTrayIcon::Context:
		emit trayContextMenu();
		return;
	case QSystemTrayIcon::Trigger:
		QTimer::singleShot(QApplication::doubleClickInterval()+1, this, SLOT(trayClicked()));
		return;
	default:
		break;
	}

	if(action)
		action->exequte();
}

void QompTrayIcon::trayClicked()
{
	if(lastClick_.msecsTo(QTime::currentTime()) > QApplication::doubleClickInterval()) {
		QompTrayAction *action = actionForType(Options::instance()->getOption(OPTION_TRAY_LEFT_CLICK).toInt());
		if(action)
			action->exequte();
	}
}

bool QompTrayIcon::eventFilter(QObject *o, QEvent *e)
{
	if(o == icon_ && e->type() == QEvent::Wheel) {
		QWheelEvent *we = static_cast<QWheelEvent*>(e);
		int delta = ((we->delta() / 8) / 1.5);
		emit trayWheeled(delta);
	}
	return QObject::eventFilter(o, e);
}

#include "qomptrayicon.moc"
