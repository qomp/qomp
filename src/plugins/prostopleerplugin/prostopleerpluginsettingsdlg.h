#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>

namespace Ui {
class ProstoPleerPluginSettingsDlg;
}

class ProstoPleerPluginSettingsDlg : public QDialog
{
	Q_OBJECT
	
public:
	ProstoPleerPluginSettingsDlg(QWidget *parent = 0);
	~ProstoPleerPluginSettingsDlg();
	
private:
	Ui::ProstoPleerPluginSettingsDlg *ui;
};

#endif // SETTINGSDLG_H
