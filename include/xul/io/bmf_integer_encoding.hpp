#pragma once

/**
 * @file
 * @brief bmf(binary message format)
 * Use Dlugosz' Variable-Length Integer Encoding Revision 2 (http://www.dlugosz.com/ZIP2/VLI.html)
 * [0 xxxxxx]     [0,2^6) 0-63
 * [10 xxxxx],1B  [2^6, 2^13) 64-8191,[64,8K)
 * [110 xxxx],2B  [2^13, 2^20), 8192-1048575,[8K,1M)
 * [1110 xxx],3B  [2^20, 2^27), 1048576-134217727,[1M,128M)
 * [11110 xx],4B  [2^27, 2^34), 134217728-17179869183,[128M,16G)
 * [11111 ff] 
 * [11111 00],5B  [2^34, 2^40)
 * [11111 01],6B  [2^40, 2^48)
 * [11111 10],7B  [2^48, 2^56)
 * [11111 11],8B  [2^56, 2^64)
 */

#include <xul/io/input_stream.hpp>
#include <xul/io/output_stream.hpp>
#include <stddef.h>
#include <stdint.h>


namespace xul {


class bmf_integer_encoding
{
public:
    bmf_integer_encoding() { }
    ~bmf_integer_encoding() { }

    static void encode(int64_t val, output_stream& os)
    {
        uint8_t buf[9];
        int pos = -1;
        uint64_t remaining;
        uint8_t signFlag;
        if (val >= 0)
        {
            remaining = val;
            signFlag = 0x80;
        }
        else
        {
            remaining = -val;
            signFlag = 0;
        }
        for (int i = 0; i < 9; ++i)
        {
            if (0 == remaining || remaining <= get_byte_limits()[i])
            {
                pos = 8 - i;
                buf[pos] = signFlag | static_cast<uint8_t>(remaining) | get_flag_bytes()[i];
                break;
            }
            uint8_t lastByte = remaining & 0xFF;
            remaining >>= 8;
            buf[8 - i] = lastByte;
        }
        assert(pos >= 0);
        os.write_bytes(buf + pos, 9 - pos);
    }
    static bool decode(input_stream& is, int64_t* val)
    {
        assert(val != NULL);
        if (false == is.is_available(1))
            return false;
        uint8_t firstByte = is.read_byte();
        uint8_t flagByte = firstByte & 0x7F;
        int pos;
        for (pos = 0; pos < 8; ++pos)
        {
            if (flagByte < get_flag_bytes()[pos+1])
                break;
        }
        assert(pos <= 8);
        int64_t result = flagByte & (get_byte_limits()[pos]);
        uint8_t buf[8];
        if (pos > 0)
        {
            if (false == is.read_n(buf, pos))
                return false;
        }
        for (int i = 0; i < pos; ++i)
        {
            result <<= 8;
            result |= buf[i];
        }
        *val = (firstByte & 0x80) != 0 ? result : -result;
        return true;
    }

private:
    static const uint8_t* get_flag_bytes()
    {
        static const uint8_t flag_bytes[9] = { 0x0, 0x40, 0x60, 0x70, 0x78, 0x7C, 0x7D, 0x7E, 0x7F};
        return flag_bytes;
    }
    static const uint8_t* get_byte_limits()
    {
        static const uint8_t byte_limits[9] = { 63, 31, 15, 7, 3, 0, 0, 0, 0};
        return byte_limits;
    }
};


}


#ifdef XUL_RUN_TEST

#include <xul/util/test_case.hpp>
#include <xul/io/memory_input_stream.hpp>
#include <xul/io/memory_output_stream.hpp>
#include <assert.h>

namespace xul {

class bmf_integer_encoding_test_case : public xul::test_case
{
protected:
    virtual void do_run()
    {
        test_one(0);
        for (int i = 0, val = 1; i < 64; ++i, val << 1)
        {
            for (int j = 0; j < 32; ++j)
            {
                test_one(val + j);
                test_one(val - j);
                test_one(-val - j);
                test_one(-val + j);
            }
        }
        test_one(34360495639LL);
        test_one(-34360495639LL);
    }
    void test_one(int64_t val)
    {
        uint8_t buf[9];
        memory_output_stream os(buf, 9);
        memory_input_stream is(buf, 9);
        bmf_integer_encoding::encode(val, os);
        int64_t outval = 0;
        assert(bmf_integer_encoding::decode(is, &outval));
        assert(outval == val);
    }
};
XUL_TEST_SUITE_REGISTRATION(bmf_integer_encoding_test_case);

}

#endif
