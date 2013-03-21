#include "qomp.h"
#include "qompmainwin.h"

#include <QEvent>
#include <QApplication>
#include <QtPlugin>


Qomp::Qomp(QObject *parent) :
	QObject(parent)
{
	mainWin_ = new QompMainWin();
	mainWin_->installEventFilter(this);

	mainWin_->show();
}

Qomp::~Qomp()
{
	delete mainWin_;
}

void Qomp::init()
{
}

bool Qomp::eventFilter(QObject *obj, QEvent *e)
{
	if(obj == mainWin_) {
		if(e->type() == QEvent::Close) {
			e->accept();
			qApp->exit();
			return true;
		}
	}

	return QObject::eventFilter(obj, e);
}
