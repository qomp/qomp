#include "urlplugin.h"

#include <QInputDialog>
#include <QtPlugin>

UrlPlugin::UrlPlugin()
{
}

TuneList UrlPlugin::getTunes()
{
	TuneList list;
	bool ok = false;
	QString url = QInputDialog::getText(0, tr("Input url"), "URL:",QLineEdit::Normal, "", &ok);
	if(ok) {
		Tune tune;
		tune.url = url;
		list.append(tune);
	}

	return list;
}

Q_EXPORT_PLUGIN2(urlplugin, UrlPlugin)
