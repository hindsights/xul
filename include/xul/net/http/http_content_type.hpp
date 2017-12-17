#pragma once

#include <string>


namespace xul {


class http_content_type
{
public:
    static const char* m3u8()
    {
        return "application/x-mpegURL";
    }
    static const char* ts_video()
    {
        return "video/MP2T";
    }
    static const char* mp4_video()
    {
        return "video/mp4";
    }
    static const char* plain_text()
    {
        return "text/plain";
    }
    static const char* xml()
    {
        return "text/xml";
    }
    static const char* html()
    {
        return "text/html";
    }
    static const char* json()
    {
        return "application/json";
    }
    static const char* octet_stream()
    {
        return "application/octet-stream";
    }
};


}
