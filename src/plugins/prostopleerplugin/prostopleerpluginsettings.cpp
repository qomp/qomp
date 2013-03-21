#include "prostopleerpluginsettings.h"
#include "ui_prostopleerpluginsettings.h"

#include "options.h"
#include "prostopleerplugindefines.h"

ProstopleerPluginSettings::ProstopleerPluginSettings(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ProstopleerPluginSettings)
{
	ui->setupUi(this);
	restore();
}

ProstopleerPluginSettings::~ProstopleerPluginSettings()
{
	delete ui;
}

void ProstopleerPluginSettings::apply()
{
	Options::instance()->setOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN, ui->le_login->text());
	Options::instance()->setOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD, ui->le_pass->text());
}


void ProstopleerPluginSettings::restore()
{
	ui->le_login->setText(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_LOGIN).toString());
	ui->le_pass->setText(Options::instance()->getOption(PROSTOPLEER_PLUGIN_OPTION_PASSWORD).toString());
}
