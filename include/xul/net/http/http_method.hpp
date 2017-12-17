#pragma once


#define XUL_HTTP_GET "GET"
#define XUL_HTTP_HEAD "HEAD"
#define XUL_HTTP_POST "POST"
#define XUL_HTTP_PUT "PUT"


namespace xul {


class http_method
{
public:
    static const char* get()
    {
        return "GET";
    }
    static const char* put()
    {
        return "PUT";
    }
    static const char* head()
    {
        return "HEAD";
    }
    static const char* post()
    {
        return "POST";
    }
};


}
