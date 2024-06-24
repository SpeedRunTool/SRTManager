#ifndef SRTMGR_COMMON_H
#define SRTMGR_COMMON_H

#ifdef DEBUG
static const bool IsDebug = true;
#else
static const bool IsDebug = false;
#endif

#ifndef LIBRARY_EXPORT_API
// Windows Static or non-Windows
#if defined(BUILD_STATIC_LIBS) || !defined(WIN32) || !defined(WIN64)
#define LIBRARY_EXPORT_API
// Windows Shared (Build/Export)
#elif defined(BUILD_SHARED_LIBS)
#define LIBRARY_EXPORT_API __declspec(dllexport)
// Windows Shared (Usage/Import)
#else
#define LIBRARY_EXPORT_API __declspec(dllimport)
#endif
#endif

#ifndef PACKED_DATA
#define PACKED_DATA __attribute__((packed))
#endif

#ifndef CONCATENATION
#define CONCATENATION(left, right) left##right
#endif

#ifndef DEFINE_CONCATENATION
#define DEFINE_CONCATENATION(left, right) CONCATENATION(left, right)
#endif

#ifndef NAMEOF
#define NAMEOF(name) #name
#endif

// #ifdef __GNUC__
// #define UNUSED(x) UNUSED_##x __attribute__((__unused__))
// #else
// #define UNUSED(x) UNUSED_##x
// #endif

#ifdef __GNUC__
#define UNUSED(x) x __attribute__((__unused__))
#else
#define UNUSED(x) x
#endif

// #ifdef __GNUC__
// #define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_##x
// #else
// #define UNUSED_FUNCTION(x) UNUSED_##x
// #endif

#ifdef __GNUC__
#define UNUSED_FUNCTION(x) __attribute__((__unused__)) x
#else
#define UNUSED_FUNCTION(x) x
#endif

// Program friendly name
#ifndef SRTMGR_APP_NAME
#define SRTMGR_APP_NAME "SRT Manager"
#endif

// Major version number. This is defined at compile time so this is just a placeholder.
#ifndef SRTMGR_VERSION_MAJOR
#define SRTMGR_VERSION_MAJOR 0
#endif

// Minor version number. This is defined at compile time so this is just a placeholder.
#ifndef SRTMGR_VERSION_MINOR
#define SRTMGR_VERSION_MINOR 1
#endif

// Patch version number. This is defined at compile time so this is just a placeholder.
#ifndef SRTMGR_VERSION_PATCH
#define SRTMGR_VERSION_PATCH 0
#endif

// Build number. This is defined at compile time so this is just a placeholder.
#ifndef SRTMGR_VERSION_BUILD
#define SRTMGR_VERSION_BUILD 0
#endif

#endif
