#pragma once

#include <xul/lang/object.hpp>
#include <stdint.h>
#include <stddef.h>


namespace xul {


class buffer : public object
{
public:
    typedef uint8_t element_type;
    typedef uint8_t* iterator;
    typedef const uint8_t* const_iterator;

    virtual void reserve(size_t size) = 0;

    virtual void resize(size_t size) = 0;
    virtual void ensure_size(size_t size) = 0;

    virtual void assign( const element_type& elem ) = 0;

    virtual void assign( const buffer& src ) = 0;

    virtual void assign(const element_type* src, size_t size) = 0;

    virtual void append(const element_type* src, size_t size) = 0;

    virtual void append(size_t size, const element_type& elem) = 0;

    virtual void append(const buffer& src) = 0;

    virtual void clear() = 0;

    virtual void swap(buffer& b) = 0;

    virtual size_t size() const = 0;

    virtual size_t capacity() const = 0;

    virtual bool empty() const = 0;

    virtual element_type* data() = 0;

    virtual const element_type* data() const = 0;


    virtual element_type* begin() = 0;
    virtual const element_type* begin() const = 0;
    virtual element_type* end() = 0;
    virtual const element_type* end() const = 0;

    //virtual element_type operator[](size_t index) const = 0;
    //virtual element_type& operator[](size_t index) = 0;
};


inline void swap(buffer & a, buffer& b) // never throws
{
    a.swap(b);
}


buffer* create_buffer();


}


#include <xul/data/byte_buffer.hpp>
