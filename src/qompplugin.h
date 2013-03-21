#ifndef QOMPPLUGIN_H
#define QOMPPLUGIN_H

#include "tune.h"

#include <QObject>

class QompPlugin
{
public:
	virtual QString name() const = 0;
	virtual QString version() const = 0;
	virtual QString description() const = 0;
	virtual TuneList getTunes() = 0;
};

Q_DECLARE_INTERFACE(QompPlugin, "Qomp.QompPlugin/0.1")

#endif // QOMPPLUGIN_H
