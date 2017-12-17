#pragma once


#define XUL_LIMIT_MIN(val, minVal) do { if ((val) < (minVal)) (val) = (minVal); assert((val) >= (minVal)); } while (false)

#define XUL_LIMIT_MAX(val, maxVal) do { if ((val) > (maxVal)) (val) = (maxVal); assert((val) <= (maxVal)); } while (false)

#define XUL_LIMIT_MIN_MAX(val, minVal, maxVal) \
    do { \
    if ((val) > (maxVal)) (val) = (maxVal); \
    if ((val) < (minVal)) (val) = (minVal); \
    assert((val) >= (minVal)); \
    assert((val) <= (maxVal)); \
    } while (false)

