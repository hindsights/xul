#pragma once


#define XUL_READ_MEMORY(buf, type) (*reinterpret_cast<const type*>(buf))

#define XUL_WRITE_MEMORY(buf, val, type) do { *reinterpret_cast<type*>(buf) = (val); } while (false)


