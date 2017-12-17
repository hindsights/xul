#pragma once

#include <stdint.h>

#pragma warning(disable: 4127)


#define MAKE_ULONGLONG(low, high)        (uint64_t) ( ( ((uint64_t)(high)) << 32 ) | ((uint32_t)(low)) )
#define MAKE_QWORD(low, high)        (uint64_t) ( ( ((uint64_t)(high)) << 32 ) | ((uint32_t)(low)) )


#define MAKE_DWORD(low, high)      ((uint32_t)(((uint16_t)(low)) | ((uint32_t)((uint16_t)(high))) << 16))


#define LO_DWORD(x)                ( (uint32_t) ( (x) & LOWPART_LONGLONG ) )

#define HI_DWORD(x)                ( (uint32_t) ( ( (x) & HIGHPART_LONGLONG ) >> 32 ) )


const uint64_t LOWPART_LONGLONG        = 0x00000000FFFFFFFFULL;
const uint64_t HIGHPART_LONGLONG    = 0xFFFFFFFF00000000ULL;

