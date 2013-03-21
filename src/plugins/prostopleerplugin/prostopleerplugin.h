#ifndef PROSTOPLEERPLUGIN_H
#define PROSTOPLEERPLUGIN_H

#include "qompplugin.h"
#include "prostopleerplugindefines.h"

#ifndef QT_STATICPLUGIN
#define QT_STATICPLUGIN
#endif

class ProstoPleerPlugin : public QObject, public QompPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin)
public:
	ProstoPleerPlugin();
	virtual QString name() const { return PROSTOPLEER_PLUGIN_NAME; }
	virtual QString version() const { return PROSTOPLEER_PLUGIN_VERSION; }
	virtual QString description() const { return QString(); }
	virtual TuneList getTunes();
};

#endif // PROSTOPLEERPLUGIN_H
