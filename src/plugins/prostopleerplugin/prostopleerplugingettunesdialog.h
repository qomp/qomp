#ifndef GETTUNESDIALOG_H
#define GETTUNESDIALOG_H

#include <QDialog>

#include "tune.h"

namespace Ui {
class ProstoPleerPluginGetTunesDialog;
}
class QNetworkAccessManager;
class QNetworkReply;
class ProstopleerModel;
class QModelIndex;
class QEvent;

class ProstoPleerPluginGetTunesDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit ProstoPleerPluginGetTunesDialog(QWidget *parent = 0);
	~ProstoPleerPluginGetTunesDialog();

	TuneList getTunes() const;

public slots:
	virtual void accept();

private slots:
	void doSearch();
	void searchFinished();
	void urlFinished();
	void loginFinished();
	void itemSelected(const QModelIndex& index);
	void itemActivated(const QModelIndex& index);

	void actPrevActivated();
	void actNextActivated();
	void doSettings();

protected:
	bool eventFilter(QObject *o, QEvent *e);
	void keyPressEvent(QKeyEvent *e);

private:
	void doLogin();
	void doSearchStepTwo();
	
private:
	Ui::ProstoPleerPluginGetTunesDialog *ui;
	TuneList tunes_;
	QNetworkAccessManager* nam_;
	ProstopleerModel* model_;
	QString lastSearchStr_;
};

#endif // GETTUNESDIALOG_H
