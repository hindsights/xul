#pragma once

#include <string>
#include <vector>
#include <stdint.h>
#include <assert.h>


namespace xul {


class hasher
{
public:
    virtual int get_max_digest_length() const = 0;
    virtual void reset() = 0;
    virtual void update(const uint8_t* data, int size) = 0;
    virtual int finalize(uint8_t* digest, int size) = 0;

    void update_string(const char* data, int size)
    {
        update(reinterpret_cast<const uint8_t*>(data), size);
    }
    void update_string(const std::string& s)
    {
        update_string(s.data(), s.size());
    }
    std::string finalize_to_string()
    {
        int size = get_max_digest_length();
        std::string s;
        s.resize(size);
        int retsize = finalize(reinterpret_cast<uint8_t*>(&s[0]), s.size());
        if (retsize < 0)
        {
            assert(false);
            return std::string();
        }
        assert(retsize <= size);
        s.resize(retsize);
        return s;
    }
};


template <typename HasherT, typename DigestT>
class hasher_mixin
{
public:
    typedef DigestT digest_type;

    static std::string hash_to_string(const uint8_t* data, int size)
    {
        HasherT hasher;
        hasher.update(data, size);
        return hasher.finalize_to_string();
    }
    static std::string hash_to_string(const char* data, int size)
    {
        return hash_to_string(reinterpret_cast<const uint8_t*>(data), size);
    }
    static std::string hash_to_string(const std::string& s)
    {
        return hash_to_string(s.data(), s.size());
    }
    static std::string hash_to_string(const std::vector<uint8_t>& data)
    {
        return hash_to_string(data.data(), data.size());
    }

    static digest_type hash(const uint8_t* data, int size)
    {
        HasherT hasher;
        hasher.update(data, size);
        return hasher.finalize();
    }
    static digest_type hash(const char* data, int size)
    {
        return hash(reinterpret_cast<const uint8_t*>(data), size);
    }
    static digest_type hash(const std::string& s)
    {
        return hash(s.data(), s.size());
    }
    static digest_type hash(const std::vector<uint8_t>& data)
    {
        return hash(data.data(), data.size());
    }
};

}
