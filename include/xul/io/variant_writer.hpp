#pragma once

#include <boost/noncopyable.hpp>


#include <xul/io/data_output_stream.hpp>
#include <string>
#include <vector>

#if defined(_MSC_VER)
#pragma push_macro("max")
#undef max
#endif
#include <limits>



namespace xul {


template <typename LengthT, typename CharT, typename TraitsT, typename AllocT>
class variant_string_writer
{
public:
    const std::basic_string<CharT, TraitsT, AllocT>& data;
    explicit variant_string_writer( const std::basic_string<CharT, TraitsT, AllocT>& s ) : data( s )
    {
        assert( data.size() < std::numeric_limits<LengthT>::max() );
    }
};

template <typename LengthT, typename CharT, typename TraitsT, typename AllocT>
inline data_output_stream& operator<<( data_output_stream& os, const variant_string_writer<LengthT, CharT, TraitsT, AllocT>& writer )
{
    LengthT len = static_cast<LengthT>( writer.data.size() );
    return os << len << writer.data;
}



template <typename LengthT, typename AllocT, typename T>
class variant_vector_writer
{
public:
    const std::vector<T, AllocT>& items;
    explicit variant_vector_writer( const std::vector<T, AllocT>& d ) : items( d )
    {
        assert( items.size() < std::numeric_limits<LengthT>::max() );
    }
};

template <typename LengthT, typename AllocT, typename T>
inline data_output_stream& operator<<( data_output_stream& os, const variant_vector_writer<LengthT, AllocT, T>& writer )
{
    LengthT len = static_cast<LengthT>( writer.items.size() );
    os << len;
    os.write_vector( writer.items );
    return os;
}


template <typename LengthT, typename T>
class variant_item_writer
{
public:
    const T& item;
    explicit variant_item_writer( const T& d ) : item( d )
    {
    }
};

template <typename LengthT, typename T>
inline data_output_stream& operator<<( data_output_stream& os, const variant_item_writer<LengthT, T>& writer )
{
    LengthT len = static_cast<LengthT>( writer.item.get_object_size() );
    return os << len << writer.item;
}


template <typename LengthT, typename T, typename AllocT>
class variant_buffer_writer
{
public:
    const basic_buffer<T, AllocT>& items;
    explicit variant_buffer_writer( const basic_buffer<T, AllocT>& d ) : items( d )
    {
        assert( items.size() < std::numeric_limits<LengthT>::max() );
    }
};

template <typename LengthT, typename T, typename AllocT>
inline data_output_stream& operator<<( data_output_stream& os, const variant_buffer_writer<LengthT, T, AllocT>& writer )
{
    LengthT len = static_cast<LengthT>( writer.items.size() );
    os << len;
    os.write_buffer( writer.items );
    return os;
}



template <typename LengthT>
class variant_writer
{
public:

    /// string
    template <typename CharT, typename TraitsT, typename AllocT>
    static variant_string_writer<LengthT, CharT, TraitsT, AllocT> make( const std::basic_string<CharT, TraitsT, AllocT>& data )
    {
        return variant_string_writer<LengthT, CharT, TraitsT, AllocT>( data );
    }

    /// vector
    template <typename AllocT, typename T>
    static variant_vector_writer<LengthT, AllocT, T> make( const std::vector<T, AllocT>& data )
    {
        return variant_vector_writer<LengthT, AllocT, T>( data );
    }


    /// buffer
    template <typename T, typename AllocT>
    static variant_buffer_writer<LengthT, T, AllocT> make( const basic_buffer<T, AllocT>& data )
    {
        return variant_buffer_writer<LengthT, T, AllocT>( data );
    }

    /// buffer
    template <typename T>
    static variant_item_writer<LengthT, T> make_item_writer( const T& data )
    {
        return variant_item_writer<LengthT, T>( data );
    }

};

typedef variant_writer<uint8_t> variant_writer_uint8;
typedef variant_writer<uint16_t> variant_writer_uint16;
typedef variant_writer<uint32_t> variant_writer_uint32;



}


#if defined(_MSC_VER)
#pragma pop_macro("max")
#endif


