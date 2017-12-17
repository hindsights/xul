#pragma once


#include <xul/io/data_input_stream.hpp>
#include <xul/data/buffer.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <vector>


namespace xul {


template <typename LengthT, typename CharT, typename TraitsT, typename AllocT>
class variant_string_reader
{
public:
    std::basic_string<CharT, TraitsT, AllocT>& data;
    size_t max_count;
    explicit variant_string_reader( std::basic_string<CharT, TraitsT, AllocT>& s, size_t maxCount ) : data( s ), max_count( maxCount ) { }
};


template <typename LengthT, typename T, typename AllocT>
class extensible_variant_vector_reader
{
public:
    std::vector<T, AllocT>& items;
    size_t max_count;
    size_t item_size;
    explicit extensible_variant_vector_reader( std::vector<T, AllocT>& d, size_t itemSize, size_t maxCount )
        : items( d ), item_size( itemSize ), max_count( maxCount )
    {
    }
};

template <typename LengthT, typename T, typename AllocT>
class simple_variant_vector_reader
{
public:
    std::vector<T, AllocT>& items;
    size_t max_count;
    size_t item_size;
    explicit simple_variant_vector_reader( std::vector<T, AllocT>& d, size_t itemSize, size_t maxCount )
        : items( d ), item_size( itemSize ), max_count( maxCount )
    {
    }
};


template <typename LengthT, typename T, typename AllocT>
class variant_buffer_reader
{
public:
    basic_buffer<T, AllocT>& data;
    size_t max_count;
    explicit variant_buffer_reader( basic_buffer<T, AllocT>& s, size_t maxCount  ) : data( s ), max_count( maxCount ) { }
};

template <typename LengthT, typename T>
class variant_item_reader
{
public:
    T& item;
    explicit variant_item_reader( T& s  ) : item( s ) { }
};


class variant_reader_util
{
public:
    template<typename T, typename AllocT>
    static bool read_padded_vector(data_input_stream& is, size_t count, std::vector<T, AllocT>& items, size_t itemSize)
    {
        assert( itemSize > 0 );
        if ( false == is.try_read_bytes( count * itemSize ) )
            return false;
        if ( 0 == count )
        {
            items.resize(0);
            return true;
        }
        items.resize( count );
        for ( size_t index = 0; index < count; ++index )
        {
            if ( false == items[index].read_item(is, itemSize) )
            {
                assert(false);
                return false;
            }
        }
        return true;
    }

};


template <typename LengthT>
class variant_reader
{
public:
    /// string
    template <typename CharT, typename TraitsT, typename AllocT>
    static variant_string_reader<LengthT, CharT, TraitsT, AllocT> make_string_reader( std::basic_string<CharT, TraitsT, AllocT>& data, size_t maxCount )
    {
        return variant_string_reader<LengthT, CharT, TraitsT, AllocT>( data, maxCount );
    }

    template <typename T, typename AllocT>
    static extensible_variant_vector_reader<LengthT, T, AllocT> make_extensible_vector_reader( std::vector<T, AllocT>& data, size_t itemSize, size_t maxCount )
    {
        return extensible_variant_vector_reader<LengthT, T, AllocT>( data, itemSize, maxCount );
    }

    template <typename T, typename AllocT>
    static simple_variant_vector_reader<LengthT, T, AllocT> make_simple_vector_reader( std::vector<T, AllocT>& data, size_t itemSize, size_t maxCount )
    {
        return simple_variant_vector_reader<LengthT, T, AllocT>( data, itemSize, maxCount );
    }

    template <typename T, typename AllocT>
    static variant_buffer_reader<LengthT, T, AllocT> make_buffer_reader( basic_buffer<T, AllocT>& data, size_t maxCount )
    {
        return variant_buffer_reader<LengthT, T, AllocT>( data, maxCount );
    }

    template <typename T>
    static variant_item_reader<LengthT, T> make_item_reader( T& data )
    {
        return variant_item_reader<LengthT, T>( data );
    }

};

typedef variant_reader<uint8_t> variant_reader_uint8;
typedef variant_reader<uint16_t> variant_reader_uint16;
typedef variant_reader<uint32_t> variant_reader_uint32;


template <typename LengthT, typename CharT, typename TraitsT, typename AllocT>
inline data_input_stream& operator>>( data_input_stream& is, const variant_string_reader<LengthT, CharT, TraitsT, AllocT>& reader )
{
    LengthT len = 0;
    if ( is >> len )
    {
        if ( reader.max_count > 0 && len > reader.max_count )
        {
            is.set_bad();
            return is;
        }
        is.read_string( len, reader.data );
    }
    return is;
}

template <typename LengthT, typename T, typename AllocT>
inline data_input_stream& operator>>( data_input_stream& is, const variant_buffer_reader<LengthT, T, AllocT>& reader )
{
    LengthT len = 0;
    if ( is >> len )
    {
        if ( reader.max_count > 0 && len > reader.max_count )
        {
            is.set_bad();
            return is;
        }
        is.read_buffer( len, reader.data );
    }
    return is;
}

template <typename LengthT, typename T, typename AllocT>
inline data_input_stream& operator>>( data_input_stream& is, const extensible_variant_vector_reader<LengthT, T, AllocT>& reader )
{
    LengthT len = 0;
    if ( is >> len )
    {
        if ( reader.max_count > 0 && len > reader.max_count )
        {
            is.set_bad();
            return is;
        }
        variant_reader_util::read_padded_vector( is, len, reader.items, reader.item_size);
    }
    return is;
}

template <typename LengthT, typename T, typename AllocT>
inline data_input_stream& operator>>( data_input_stream& is, const simple_variant_vector_reader<LengthT, T, AllocT>& reader )
{
    LengthT len = 0;
    if ( is >> len )
    {
        if ( reader.max_count > 0 && len > reader.max_count )
        {
            is.set_bad();
            return is;
        }
        is.read_vector( reader.items, reader.item_size, len);
    }
    return is;
}


template <typename LengthT, typename T>
inline data_input_stream& operator>>( data_input_stream& is, const variant_item_reader<LengthT, T>& reader )
{
    LengthT len = 0;
    if ( is >> len )
    {
        if ( false == reader.item.parse(is, len) )
        {
            is.set_bad();
            return is;
        }
    }
    return is;
}


}
