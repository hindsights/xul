#pragma once

#include <xul/crypto/inplace_coding.hpp>


namespace xul {


class aes_encrypter : public inplace_coding
{
public:
    virtual const uint8_t* get_key() const = 0;
    virtual int get_key_size() const = 0;
    virtual void set_key(const uint8_t* key, int bits) = 0;
    virtual void encrypt(const uint8_t* src, int size, uint8_t* dst) = 0;
};


}
