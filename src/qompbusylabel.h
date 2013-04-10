#ifndef QOMPBUSYLABEL_H
#define QOMPBUSYLABEL_H

#include <QLabel>
class QTimer;

class QompBusyLabel : public QLabel
{
	Q_OBJECT
public:
	explicit QompBusyLabel(const QString& text, QWidget *parent);
	~QompBusyLabel();
	void changeText(const QString& text);

public slots:
	void start();
	void stop();
	
protected:
	void paintEvent(QPaintEvent *);
	bool eventFilter(QObject *o, QEvent *e);

private:
	void updateGeometry();
	
private slots:
	void timeout();
	
private:
	QTimer* timer_;
	QWidget *parent_;
	int curPos_;
	QString curText_;
};

#endif // QOMPBUSYLABEL_H
