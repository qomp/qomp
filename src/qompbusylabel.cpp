#include "qompbusylabel.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QTimer>

QompBusyLabel::QompBusyLabel(QWidget *parent) :
	QLabel(parent),
	curPos_(0)
{
	setTextFormat(Qt::RichText);

	timer_ = new QTimer(this);
	connect(timer_, SIGNAL(timeout()), SLOT(timeout()));
	timer_->setInterval(100);
}

QompBusyLabel::~QompBusyLabel()
{
	timer_->stop();
}

void QompBusyLabel::changeText(const QString &text)
{
	curText_ = text;
	curPos_ = 0;
}

bool QompBusyLabel::isActive() const
{
	return timer_->isActive();
}

void QompBusyLabel::start()
{
	curPos_ = 0;
	timer_->start();
	timeout();
	show();
}

void QompBusyLabel::stop()
{
	timer_->stop();
	hide();
}

void QompBusyLabel::paintEvent(QPaintEvent *e)
{
	QLabel::paintEvent(e);
}

void QompBusyLabel::timeout()
{
	QString text = curText_.left(curPos_);
	text += QString("<span style=\"color: red;\">") + curText_.at(curPos_) + QString("</span>");
	text += curText_.right(curText_.size() - (++curPos_));
	setText(text);
	if(curPos_ == curText_.length())
		curPos_ = 0;
	update();
}
