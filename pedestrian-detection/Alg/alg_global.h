#ifndef ALG_GLOBAL_H
#define ALG_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ALG_LIBRARY)
#  define ALGSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ALGSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ALG_GLOBAL_H
