#ifndef URLPLUGIN_H
#define URLPLUGIN_H

#include "qompplugin.h"

#ifndef QT_STATICPLUGIN
#define QT_STATICPLUGIN
#endif

class UrlPlugin : public QObject, public QompPlugin
{
	Q_OBJECT
	Q_INTERFACES(QompPlugin)
public:
	UrlPlugin();	
	virtual QString name() const { return "Url"; }
	virtual QString version() const { return "0.1"; }
	virtual QString description() const { return QString(); }
	virtual TuneList getTunes();
	
};

#endif // URLPLUGIN_H
