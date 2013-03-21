#include "prostopleerpluginsettingsdlg.h"
#include "ui_prostopleerpluginsettingsdlg.h"

ProstoPleerPluginSettingsDlg::ProstoPleerPluginSettingsDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProstoPleerPluginSettingsDlg)
{
	ui->setupUi(this);
	connect(this, SIGNAL(accepted()), ui->widget, SLOT(apply()));
}

ProstoPleerPluginSettingsDlg::~ProstoPleerPluginSettingsDlg()
{
	delete ui;
}
