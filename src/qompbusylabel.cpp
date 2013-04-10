#include "qompbusylabel.h"

#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QTimer>

QompBusyLabel::QompBusyLabel(const QString &text, QWidget *parent) :
	QLabel(0, Qt::ToolTip),
	parent_(parent),
	curPos_(0),
	curText_(text)
{
//	QPalette palette = this->palette();
//	palette.setBrush(QPalette::Base, Qt::transparent);
//	setPalette(palette);
//	setAttribute(Qt::WA_TranslucentBackground, true);
	setTextFormat(Qt::RichText);

	timer_ = new QTimer(this);
	connect(timer_, SIGNAL(timeout()), SLOT(timeout()));
	timer_->setInterval(100);

	parent_->installEventFilter(this);
	connect(parent_, SIGNAL(destroyed()), SLOT(deleteLater()));
	updateGeometry();
}

QompBusyLabel::~QompBusyLabel()
{
	timer_->stop();
}

void QompBusyLabel::changeText(const QString &text)
{
	curText_ = text;
}

void QompBusyLabel::start()
{
	curPos_ = 0;
	updateGeometry();
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
	updateGeometry();
	QLabel::paintEvent(e);
}

bool QompBusyLabel::eventFilter(QObject *o, QEvent *e)
{
	if(o == parent_ && isVisible()) {
		update();
	}

	return QLabel::eventFilter(o, e);
}

void QompBusyLabel::updateGeometry()
{
	Qt::Alignment al = alignment();
	QFontMetrics fm = fontMetrics();
	QRect rect(parent_->x(), parent_->y(), fm.width(curText_), fm.height());

	if(al & Qt::AlignHCenter)
		rect.moveLeft(parent_->geometry().width()/2 - rect.width()/2);
	else if(al & Qt::AlignRight)
		rect.moveRight(parent_->geometry().right());
	if(al & Qt::AlignCenter)
		rect.moveTop(parent_->geometry().height()/2 - rect.height()/2);
	else if(al & Qt::AlignBottom)
		rect.moveBottom(parent_->geometry().bottom());

	move(parent_->mapToGlobal(rect.topLeft()));
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
