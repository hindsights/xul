#pragma once

#include <stdint.h>

#pragma warning(disable: 4127)

#define XUL_COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

