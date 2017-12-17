#pragma once

#include <string>


namespace xul {


class tribool
{
public:
    bool is_true() const { return m_value > 0; }
    bool is_false() const { return m_value < 0; }
    bool is_unknown() const { return 0 == m_value; }

    void set_true() { m_value = 1; }
    void set_false() { m_value = -1; }
    void set_unknown() { m_value = 0; }

private:
    signed char m_value;
};


}
