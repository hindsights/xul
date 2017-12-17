#pragma once

#include <utility>
#include <ostream>


namespace xul {


template <typename T1, typename T2>
struct pair
{
    T1 first;
    T2 second;

    pair(const T1& a, const T2& b) : first(a), second(b) { }
};

template <typename T1, typename T2, typename T3>
struct triple
{
    T1 first;
    T2 second;
    T3 third;

    triple(const T1& a, const T2& b, const T3& c) : first(a), second(b), third(c) { }
};


template <typename T1, typename T2, typename T3, typename T4>
struct quadruple
{
    T1 first;
    T2 second;
    T3 third;
    T4 fourth;

    quadruple(const T1& a, const T2& b, const T3& c, const T4& d) : first(a), second(b), third(c), fourth(d) { }
};


template <typename T1, typename T2>
inline pair<T1, T2> make_pair(const T1& a, const T2& b)
{
    return pair<T1, T2>(a, b);
}

template <typename T1, typename T2, typename T3>
inline triple<T1, T2, T3> make_triple(const T1& a, const T2& b, const T3& c)
{
    return triple<T1, T2, T3>(a, b, c);
}

template <typename T1, typename T2, typename T3, typename T4>
inline quadruple<T1, T2, T3, T4> make_quadruple(const T1& a, const T2& b, const T3& c, const T4& d)
{
    return quadruple<T1, T2, T3, T4>(a, b, c, d);
}


template <typename T1, typename T2>
inline xul::pair<T1, T2> make_tuple(const T1& x, const T2& y)
{
    return xul::make_pair(x, y);
}

template <typename T1, typename T2, typename T3>
inline triple<T1, T2, T3> make_tuple(const T1& x, const T2& y, const T3& z)
{
    return make_triple(x, y, z);
}

template <typename T1, typename T2, typename T3, typename T4>
inline quadruple<T1, T2, T3, T4> make_tuple(const T1& a, const T2& b, const T3& c, const T4& d)
{
    return make_quadruple(a, b, c, d);
}


template <typename T1, typename T2>
inline std::ostream& operator<<(std::ostream& os, const xul::pair<T1, T2>& val)
{
    os << "(" << val.first << "," << val.second << ")";
    return os;
}

template <typename T1, typename T2, typename T3>
inline std::ostream& operator<<(std::ostream& os, const xul::triple<T1, T2, T3>& val)
{
    os << "(" << val.first << "," << val.second << "," << val.third << ")";
    return os;
}

template <typename T1, typename T2, typename T3, typename T4>
inline std::ostream& operator<<(std::ostream& os, const xul::quadruple<T1, T2, T3, T4>& val)
{
    os << "(" << val.first << "," << val.second << "," << val.third << "," << val.fourth << ")";
    return os;
}


}
