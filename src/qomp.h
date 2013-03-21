#ifndef QOMP_H
#define QOMP_H

#include <QObject>

class QompMainWin;


class Qomp : public QObject
{
	Q_OBJECT
public:
	Qomp(QObject *parent = 0);
	~Qomp();

	void init();

private slots:

protected:
	bool eventFilter(QObject *obj, QEvent *e);

private:
	QompMainWin* mainWin_;	
};

#endif // QOMP_H
