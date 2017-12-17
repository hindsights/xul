#pragma once


namespace xul {


class amf0_types
{
public:
    enum
    {
        /**
         * number marker constant
         */
        amf0_number = 0x00,

        /**
         * boolean value marker constant
         */
        amf0_boolean = 0x01,

        /**
         * string marker constant
         */
        amf0_string = 0x02,

        /**
         * object marker constant
         */
        amf0_object = 0x03,

        /**
         * movieclip marker constant
         */
        amf0_movieclip = 0x04,

        /**
         * null marker constant
         */
        amf0_null = 0x05,

        /**
         * undefined marker constant
         */
        amf0_undefined = 0x06,

        /**
         * object reference marker constant
         */
        amf0_reference = 0x07,

        /**
         * mixed array marker constant
         */
        amf0_mixed_array = 0x08,

        /**
         * end of object marker constant
         */
        amf0_end_of_object = 0x09,

        /**
         * array marker constant
         */
        amf0_array = 0x0a,

        /**
         * date marker constant
         */
        amf0_date = 0x0b,

        /**
         * long string marker constant
         */
        amf0_long_string = 0x0c,

        /**
         * unsupported type marker constant
         */
        amf0_unsupported = 0x0d,

        /**
         * recordset marker constant
         */
        amf0_recordset = 0x0e,

        /**
         * xml marker constant
         */
        amf0_xml = 0x0f,

        /**
         * class marker constant
         */
        amf0_class_object = 0x10,

        /**
         * object marker constant (for amf3)
         */
        amf0_amf3_object = 0x11
    };

    enum
    {
        /**
         * max string length
         */
        amf0_long_string_length = 65535
    };

};


class amf3_types
{
public:
    enum
    {
        amf3_min_integer_value = -268435456,

        /**
         * maximum possible value for integer number encoding.
         */
        amf3_max_integer_value = 268435455,

        /**
         * max string length
         */
        amf3_long_string_length = 65535
    };

    enum
    {
        /**
         * null marker
         */
        amf3_null = 0x01,

        /**
         * boolean false marker
         */
        amf3_boolean_false = 0x02,

        /**
         * boolean true marker
         */
        amf3_boolean_true = 0x03,

        /**
         * integer marker
         */
        amf3_integer = 0x04,

        /**
         * number marker
         */
        amf3_number = 0x05,

        /**
         * string marker
         */
        amf3_string = 0x06,

        // TODO m.j.m hm..not defined on site, says it's only XML type, so i'll
        // assume it is for the time being..
        amf3_xml_special = 0x07,

        /**
         * date marker
         */
        amf3_date = 0x08,

        /**
         * array start marker
         */
        amf3_array = 0x09,

        /**
         * object start marker
         */
        amf3_object = 0x0a,

        /**
         * xml start marker
         */
        amf3_xml = 0x0b,

        /**
         * bytearray marker
         */
        amf3_bytearray = 0x0c

    };

    enum
    {
        /**
         * Property list encoding.
         *
         * The remaining integer-data represents the number of class members
         * that exist. The property names are read as string-data. The values
         * are then read as AMF3-data.
         */
        amf3_object_property = 0x00,

        /**
         * Externalizable object.
         *
         * What follows is the value of the "inner" object, including type code.
         * This value appears for objects that implement IExternalizable, such
         * as ArrayCollection and ObjectProxy.
         */
        amf3_object_externalizable = 0x01,

        /**
         * Name-value encoding.
         *
         * The property names and values are encoded as string-data followed by
         * AMF3-data until there is an empty string property name. If there is
         * a class-def reference there are no property names and the number of
         * values is equal to the number of properties in the class-def.
         */
        amf3_object_value = 0x02,

        /**
         * Proxy object.
         */
        amf3_object_proxy = 0x03
    };
};


}
