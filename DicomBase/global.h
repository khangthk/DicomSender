#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(DICOMBASE_LIBRARY)
#  define DICOMBASE_EXPORT Q_DECL_EXPORT
# else
#  define DICOMBASE_EXPORT Q_DECL_IMPORT
# endif
#else
# define DICOMBASE_EXPORT
#endif
