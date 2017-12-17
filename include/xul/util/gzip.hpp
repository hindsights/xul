#pragma once

#include "zlib.h"

namespace xul {

int gzip_uncompress(
    Bytef *dest,
    uLongf *destLen,
    const Bytef *source,
    uLong sourceLen)
{
    z_stream stream;
    int err;
    int gzip = 1;

    stream.next_in = (Bytef*)source;
    stream.avail_in = (uInt)sourceLen;
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
stream.next_out = dest;
    stream.avail_out = (uInt)*destLen;
    if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    if (gzip == 1)
    {
        err = inflateInit2(&stream, 47);
    }
    else
    {
        err = inflateInit(&stream);
    }
    if (err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    err = inflateEnd(&stream);
    return err;
}

}
