#ifndef VERSIONS_H
#define VERSIONS_H

#ifdef _WIN32
#define BUILD_WINDOWS	1
#else
#define BUILD_WINDOWS	0
#endif

#ifdef __APPLE__
#define BUILD_MAC	1
#else
#define BUILD_MAC	0
#endif

#ifdef __linux__
#define BUILD_LINUX	1
#else
#define BUILD_LINUX	0
#endif

#ifdef __SWITCH__
#define BUILD_SWITCH 1
#else
#define BUILD_SWITCH 0
#endif

#define BUILD_STEAM				0
#define BUILD_SDL				( 0 || BUILD_MAC || BUILD_LINUX || BUILD_SWITCH )
#define BUILD_WINDOWS_NO_SDL	( BUILD_WINDOWS && ( !BUILD_SDL ) )

#define REPORT_BUILD 0
#if REPORT_BUILD
#if BUILD_WINDOWS
#pragma message( "Building for Windows" )
#endif

#if BUILD_MAC
#pragma message( "Building for Mac OS X" )
#endif

#if BUILD_LINUX
#pragma message( "Building for Linux" )
#endif

#if BUILD_STEAM
#pragma message( "Building for Steam" )
#endif

#if BUILD_SDL
#pragma message( "Building with SDL" )
#endif

#if BUILD_WINDOWS_NO_SDL
#pragma message( "Building Windows without SDL" )
#endif
#endif // REPORT_BUILD

#endif // VERSIONS_H
