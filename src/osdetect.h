#ifndef GAIA_OSDETECT_H
#define GAIA_OSDETECT_H

//#ifdef _MSC_VER
//#  define OS_WIN32
//#else // _MSC_VER
//#  define OS_LINUX
//#endif // _MSC_VER

#if defined(Q_OS_WIN32) || defined(_MSC_VER)
#  define OS_WIN32
#else
#  ifdef Q_OS_LINUX
#    define OS_LINUX
#  else
#    ifdef Q_OS_DARWIN
#      define OS_DARWIN
#    endif
#  endif
#endif

#endif // GAIA_OSDETECT_H
