#ifndef KDCRMDATA_EXPORT_H
#define KDCRMDATA_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef KDCRMDATA_EXPORT
# if defined(MAKE_KDCRMDATA_LIB)
   /* We are building this library */
#  define KDCRMDATA_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KDCRMDATA_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KDCRMDATA_EXPORT_DEPRECATED
#  define KDCRMDATA_EXPORT_DEPRECATED KDE_DEPRECATED KDCRMDATA_EXPORT
# endif

#endif
