#pragma once

#include <xul/data/numerics.hpp>
#include <stdlib.h>
#include <stdint.h>


namespace xul {


class random
{
public:
    static void init_seed(unsigned int seed)
    {
        ::srand(seed);
    }

    random()
    {
    }

    random(unsigned int seed)
    {
        init_seed(seed);
    }

    static int next()
    {
        return ::rand();
    }

    static int next_in(int range)
    {
        return next() % range;
    }

    static uint8_t next_byte()
    {
        return static_cast<uint8_t>(next_in(0xFF));
    }
    static uint16_t next_word()
    {
        return static_cast<uint16_t>(next());
    }
    static uint32_t next_dword()
    {
        uint16_t low = next_word();
        uint16_t high = next_word();
        return numerics::make_dword(high, low);
    }
    static uint64_t next_qword()
    {
        uint32_t low = next_dword();
        uint32_t high = next_dword();
        return numerics::make_qword(high, low);
    }
    static void generate_string(std::string& s, size_t size)
    {
        assert(size < 1024 * 1024);
        s.resize(size);
        for (int i = 0; i < s.size(); ++i)
        {
            s[i] = xul::random::next_byte();
        }
    }
};


template <typename ValueT>
class random_generator;


template <>
class random_generator<uint16_t>
{
public:
    static uint16_t generate()
    {
        return random::next_word();
    }
};


template <>
class random_generator<uint32_t>
{
public:
    static uint16_t generate()
    {
        return random::next_dword();
    }
};


template <>
class random_generator<uint64_t>
{
public:
    static uint16_t generate()
    {
        return random::next_qword();
    }
};


}
