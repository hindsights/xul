#pragma once

#include <xul/lang/object.hpp>
#include <string>
#include <stdint.h>


namespace xul {


class inplace_coding : public object
{
public:
    virtual void process(uint8_t* buf, int size) = 0;
    virtual void transform(uint8_t* dst, const uint8_t* src, int size) = 0;

    void process(std::string& s)
    {
        if (s.empty())
            return;
        this->process(reinterpret_cast<uint8_t*>(&s[0]), s.size());
    }
};


class dummy_inplace_coding : public inplace_coding
{
public:
    virtual void process(uint8_t* buf, int size) {}
    virtual void transform(uint8_t* dst, const uint8_t* src, int size) {}
};


}
