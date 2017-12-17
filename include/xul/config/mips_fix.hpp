#pragma once


/// glibc extension, returns the number of processors
/// this is an faked implementation, because ucLibc does not have it
extern "C" int get_nprocs()
{
    return 1;
}

extern "C" int atexit(void (*)())
{
    return 0;
}


