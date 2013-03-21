#include "prostopleerplugin.h"
#include "prostopleerplugingettunesdialog.h"

#include <QtPlugin>


ProstoPleerPlugin::ProstoPleerPlugin()
{
}

TuneList ProstoPleerPlugin::getTunes()
{
	TuneList list;
	ProstoPleerPluginGetTunesDialog gtd;
	if(gtd.exec() == QDialog::Accepted)
		list = gtd.getTunes();
	return list;
}

Q_EXPORT_PLUGIN2(prostopleerplugin, ProstoPleerPlugin)
