#ifndef QGMF_GLOBAL_H
#define QGMF_GLOBAL_H


#if defined(QGMF_LIBRARY)

#ifdef _WIN32
#  define QGMF_PUBLIC __declspec(dllexport)
#else
#  define QGMF_PUBLIC __attribute__((visibility("default")))
#endif

#else
#ifdef _WIN32
#  define QGMF_PUBLIC __declspec(dllimport)
#else
#  define QGMF_PUBLIC
#endif

#endif

#endif // QGMF_GLOBAL_H
