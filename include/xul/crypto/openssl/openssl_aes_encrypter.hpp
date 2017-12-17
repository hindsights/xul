#pragma once

#include <xul/crypto/aes_encrypter.hpp>
#include <xul/lang/object_impl.hpp>

#include <openssl/aes.h>

#include <vector>
#include <stdint.h>



namespace xul {


class openssl_aes_cbc_encrypter : public xul::object_impl<aes_encrypter>
{
public:
    static const int AES_KEY_SIZE = 0x20;

    openssl_aes_cbc_encrypter()
    {
        memset(&m_key, 0, sizeof(AES_KEY));
    }

    virtual const uint8_t* get_key() const
    {
        return &m_raw_key[0];
    }
    virtual int get_key_size() const
    {
        return m_raw_key.size();
    }

    virtual void set_key(const uint8_t* key, int bits)
    {
        m_raw_key.assign(key, key + AES_KEY_SIZE);
        AES_set_encrypt_key(key, bits, &m_key);
    }
    virtual void encrypt(const uint8_t* src, int size, uint8_t* dst)
    {
        uint8_t iv[AES_KEY_SIZE];
        memset(iv,0,sizeof(iv));
        AES_cbc_encrypt(src, dst, size, &m_key, iv, 1);
    }
    virtual void process(uint8_t* buf, int size)
    {
        encrypt(buf, size, buf);
    }
    virtual void transform(uint8_t* dst, const uint8_t* src, int size)
    {
        encrypt(src, size, dst);
    }

private:
    AES_KEY m_key;
    std::vector<uint8_t> m_raw_key;
};


inline aes_encrypter* create_openssl_aes_cbc_encrypter()
{
    return new openssl_aes_cbc_encrypter;
}


}
