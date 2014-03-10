#ifndef QOMPBUSYLABEL_H
#define QOMPBUSYLABEL_H

#include <QLabel>
#include "libqomp_global.h"

class QTimer;

class LIBQOMPSHARED_EXPORT QompBusyLabel : public QLabel
{
	Q_OBJECT
public:
	explicit QompBusyLabel(QWidget *parent = 0);
	~QompBusyLabel();
	void changeText(const QString& text);
	bool isActive() const;

public slots:
	void start();
	void stop();
	
protected:
	void paintEvent(QPaintEvent *);
	
private slots:
	void timeout();
	
private:
	QTimer* timer_;
	int curPos_;
	QString curText_;
};

#endif // QOMPBUSYLABEL_H
