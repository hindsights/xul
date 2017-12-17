#pragma once


#if defined(_MSC_VER)
#  define XUL_MSVC _MSC_VER
#  pragma warning(error:4715)
#  define XUL_EXPORT __declspec(dllexport)
#  define XUL_PUBLIC

#elif defined(__GNUC__)
#  define XUL_GCC __GNUC__
#  define __cdecl __attribute__((__cdecl__))
#  define __stdcall __attribute__((__stdcall__))
#  define __fastcall __attribute__((__fastcall__))
#  define XUL_EXPORT __attribute__((visibility("default")))
#  define XUL_PUBLIC __attribute__((visibility("default")))

#else
#error "unsupported compiler"

#endif
