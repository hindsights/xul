#pragma once

#include <xul/crypto/hasher.hpp>
#include <xul/data/big_number.hpp>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <stdint.h>


namespace xul {


template <
    typename ContextT,
    typename DigestT,
    int (*FUNC_Init)(ContextT*),
    int (*FUNC_Update)(ContextT*, const void*, size_t),
    int (*FUNC_Finailize)(unsigned char*, ContextT*)
>
class openssl_hasher_impl
    : public hasher
    , public hasher_mixin<openssl_hasher_impl<ContextT, DigestT, FUNC_Init, FUNC_Update, FUNC_Finailize>, DigestT>
{
public:
    typedef DigestT digest_type;
    static const int digest_length = digest_type::byte_size;

    openssl_hasher_impl()
    {
        reset();
    }

    virtual int get_max_digest_length() const
    {
        return digest_length;
    }
    virtual void reset()
    {
        FUNC_Init(&m_context);
    }
    virtual void update(const uint8_t* data, int size)
    {
        FUNC_Update(&m_context, data, size);
    }
    virtual int finalize(uint8_t* digest, int size)
    {
        if (size < digest_length)
            return 0;
        FUNC_Finailize(digest, &m_context);
        return digest_length;
    }

    void update(const char* data, int size)
    {
        FUNC_Update(&m_context, reinterpret_cast<const uint8_t*>(data), size);
    }
    digest_type finalize()
    {
        digest_type ret;
        FUNC_Finailize(&ret[0], &m_context);
        return ret;
    }

private:
    ContextT m_context;
};

typedef openssl_hasher_impl<MD5_CTX, uint128, MD5_Init, MD5_Update, MD5_Final> openssl_md5_hasher;
typedef openssl_hasher_impl<SHA_CTX, uint160, SHA1_Init, SHA1_Update, SHA1_Final> openssl_sha1_hasher;
typedef openssl_hasher_impl<SHA256_CTX, uint256, SHA256_Init, SHA256_Update, SHA256_Final> openssl_sha256_hasher;
typedef openssl_hasher_impl<SHA512_CTX, uint512, SHA512_Init, SHA512_Update, SHA512_Final> openssl_sha512_hasher;

typedef openssl_hasher_impl<RIPEMD160_CTX, uint160, RIPEMD160_Init, RIPEMD160_Update, RIPEMD160_Final> openssl_ripemd160_hasher;


class openssl_hasher
{
public:
    static uint128 md5(const uint8_t* data, int size)
    {
        return openssl_md5_hasher::hash(data, size);
    }
    static uint128 md5(const char* data, int size)
    {
        return openssl_md5_hasher::hash(data, size);
    }

    static uint160 sha1(const uint8_t* data, int size)
    {
        return openssl_sha1_hasher::hash(data, size);
    }
    static uint160 sha1(const char* data, int size)
    {
        return openssl_sha1_hasher::hash(data, size);
    }

    static uint256 sha256(const uint8_t* data, int size)
    {
        return openssl_sha256_hasher::hash(data, size);
    }
    static uint256 sha256(const char* data, int size)
    {
        return openssl_sha256_hasher::hash(data, size);
    }

    static uint512 sha512(const char* data, int size)
    {
        return openssl_sha512_hasher::hash(data, size);
    }
    static uint512 sha512(const uint8_t* data, int size)
    {
        return openssl_sha512_hasher::hash(data, size);
    }
};


}
