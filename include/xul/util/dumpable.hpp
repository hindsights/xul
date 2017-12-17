#pragma once


namespace xul {


class structured_writer;


class dump_level
{
public:
    enum
    {
        none = 0,
        summary = 10,
        skeleton = 20,
        details = 100,
        full = 1000,
    };
};


class dumpable
{
public:
    virtual void dump(structured_writer* writer, int level) const = 0;
};


}
