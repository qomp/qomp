#ifndef LIBQOMP_GLOBAL_H
#define LIBQOMP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBQOMP_LIBRARY)
#  define LIBQOMPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBQOMPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBQOMP_GLOBAL_H
