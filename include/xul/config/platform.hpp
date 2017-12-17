#pragma once


#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
// linux, also other platforms (Hurd etc) that use GLIBC, should these really have their own config headers though?
#  define XUL_LINUX

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
// BSD:
#  define XUL_BSD

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// win32:
#  define XUL_WINDOWS

#elif defined sun || defined __sun
#  define XUL_SOLARIS

#elif defined(__APPLE__) && defined(__MACH__)

#  define XUL_MACH

#else
#error "unsupported platform"

#endif

