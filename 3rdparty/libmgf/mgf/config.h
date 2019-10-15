#ifndef __MGFP_INCLUDE_CONFIG_H__
#define __MGFP_INCLUDE_CONFIG_H__

#ifdef _MSC_VER
#define NOMINMAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define VERSION 2.1.1

#ifdef _WIN32
#define VC_EXTRALEAN
#include <windows.h>
#define MGFP_EXPORT __declspec( dllexport )
/* Disable a template related MSVC warning.
   See: http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html */
#pragma warning( disable: 4251 )
#else
#define MGFP_EXPORT
#endif

#endif // __MGFP_INCLUDE_CONFIG_H__
