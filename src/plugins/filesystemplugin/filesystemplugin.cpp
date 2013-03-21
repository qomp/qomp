#include "filesystemplugin.h"
#include "options.h"

#include <QFileDialog>
#include <QtPlugin>

FilesystemPlugin::FilesystemPlugin()
{
}

TuneList FilesystemPlugin::getTunes()
{
	QStringList files = QFileDialog::getOpenFileNames(0, tr("Select file(s)"),
					Options::instance()->getOption("filesystemplugin.lastdir", QDir::homePath()).toString(),
					"*mp3 *ogg *wav *flac");

	if(!files.isEmpty()) {
		QFileInfo fi (files.first());
		Options::instance()->setOption("filesystemplugin.lastdir", fi.dir().path());
	}
	TuneList list;
	foreach(const QString& file, files) {
		Tune tune;
		tune.file = file;
		list.append(tune);
	}
	return list;
}

Q_EXPORT_PLUGIN2(filesystemplugin, FilesystemPlugin)
