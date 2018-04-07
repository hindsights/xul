#pragma once

#include <xul/data/big_number.hpp>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace xul {




class openssl_sha256_hasher
{
public:
    typedef uint256 digest_type;

    openssl_sha256_hasher()
    {
        reset();
    }
    void reset()
    {
        SHA256_Init(&m_context);
    }
    void update(const void* data, int size)
    {
        SHA256_Update(&m_context, data, size);
    }
    uint256 finalize()
    {
        uint256 ret;
        SHA256_Final(&ret[0], &m_context);
        return ret;
    }

    static uint256 hash(const void* data, int size)
    {
        openssl_sha256_hasher hasher;
        hasher.update(data, size);
        return hasher.finalize();
    }

private:
    SHA256_CTX m_context;
};

class openssl_hasher
{
public:
    static uint128 md5(const void* data, int size)
    {
        uint128 ret;
        MD5_CTX context;
        MD5_Init(&context);
        MD5_Update(&context, data, size);
        MD5_Final(&ret[0], &context);
        return ret;
    }
    static uint160 sha1(const void* data, int size)
    {
        uint160 ret;
        SHA_CTX context;
        SHA1_Init(&context);
        SHA1_Update(&context, data, size);
        SHA1_Final(&ret[0], &context);
        return ret;
    }
    static uint256 sha256(const void* data, int size)
    {
        return openssl_sha256_hasher::hash(data, size);
    }
    static uint512 sha512(const void* data, int size)
    {
        uint512 ret;
        SHA512_CTX context;
        SHA512_Init(&context);
        SHA512_Update(&context, data, size);
        SHA512_Final(&ret[0], &context);
    }
};


}
