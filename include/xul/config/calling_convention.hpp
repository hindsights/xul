#pragma once


#if !defined(_XUL_DISABLE_CALLING_CONVENTION_MACRO)

#ifndef _MSC_VER
#define __stdcall __attribute__((stdcall))
#endif

#ifndef _MSC_VER
#define __cdecl __attribute__((cdecl))
#endif

#ifndef _MSC_VER
#define __fastcall __attribute__((fastcall))
#endif

#endif
