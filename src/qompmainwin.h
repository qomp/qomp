#ifndef QOMPMAINWIN_H
#define QOMPMAINWIN_H

#include <QMainWindow>

namespace Ui {
	class QompMainWin;
}

class QompPlayer;
class PlayListModel;
class QModelIndex;
class QNetworkAccessManager;

class QompMainWin : public QMainWindow
{
	Q_OBJECT
	
public:
	QompMainWin(QWidget *parent = 0);
	~QompMainWin();

private slots:
	void actPlayActivated();
	void actPrevActivated();
	void actNextActivated();
	void actStopActivated();
	void actOpenActivated();
	void actClearActivated();
	void mediaActivated(const QModelIndex& index);
	void mediaClicked(const QModelIndex& index);
	void updatePlayIcon();
	void setCurrentPosition(qint64 ms);
	void playNext();
	
private:
	Ui::QompMainWin *ui;
	QompPlayer* player_;
	PlayListModel* model_;
	QNetworkAccessManager* nam_;
};

#endif // QOMPMAINWIN_H
