#pragma once


#define UPNP_HTTPMU_HOST_ADDRESS "239.255.255.250"
#define UPNP_HTTPMU_HOST_PORT 1900
    //#define SEARCH_REQUEST_DEVICE "urn:schemas-upnp-org:service:%s"

#if 0
#define UPNP_SEARCH_REQUEST_STRING "M-SEARCH * HTTP/1.1\r\n"            \
        "ST: UPnP:rootdevice\r\n"             \
        "MX: 3\r\n"                          \
        "Man: \"ssdp:discover\"\r\n"          \
        "HOST: 239.255.255.250:1900\r\n"     \
"\r\n"
#else
#define UPNP_SEARCH_REQUEST_STRING "M-SEARCH * HTTP/1.1\r\n"            \
"ST: urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n"             \
"MX: 3\r\n"                          \
"Man: \"ssdp:discover\"\r\n"          \
"HOST: 239.255.255.250:1900\r\n"     \
"\r\n"
#endif

#define HTTP_OK "200 OK"


        /******************************************************************
        ** Device and Service  Defines                                                 *
        *******************************************************************/

#define UPNP_DEVICE_TYPE_1   "urn:schemas-upnp-org:device:InternetGatewayDevice:1"
#define UPNP_DEVICE_TYPE_2   "urn:schemas-upnp-org:device:WANDevice:1"
#define UPNP_DEVICE_TYPE_3   "urn:schemas-upnp-org:device:WANConnectionDevice:1"

#define UPNP_SERVICE_WANIP  "urn:schemas-upnp-org:service:WANIPConnection:1"
#define UPNP_SERVICE_WANPPP "urn:schemas-upnp-org:service:WANPPPConnection:1"




        /******************************************************************
        ** Action Defines                                                 *
        *******************************************************************/
#define UPNP_HTTP_HEADER_ACTION "POST %s HTTP/1.1\r\n"                         \
        "HOST: %s:%u\r\n"                                  \
        "SOAPACTION:\"%s#%s\"\r\n"                           \
        "CONTENT-TYPE: text/xml ; charset=\"utf-8\"\r\n"\
        "Content-Length: %d \r\n\r\n"

#define UPNP_SOAP_ACTION  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"     \
        "<s:Envelope xmlns:s="                               \
        "\"http://schemas.xmlsoap.org/soap/envelope/\" "     \
        "s:encodingStyle="                                   \
        "\"http://schemas.xmlsoap.org/soap/encoding/\">\r\n" \
        "<s:Body>\r\n"                                       \
        "<u:%s xmlns:u=\"%s\">\r\n%s"         \
        "</u:%s>\r\n"                                        \
        "</s:Body>\r\n"                                      \
        "</s:Envelope>\r\n"

#define UPNP_PORT_MAPPING_LEASE_TIME "0"
#define UPNP_PORT_MAPPING_DESCRIPTION "UPNP PORT MAPPING TOOL"

#define UPNP_ADD_PORT_MAPPING_PARAMS "<NewRemoteHost></NewRemoteHost>\r\n"      \
        "<NewExternalPort>%u</NewExternalPort>\r\n"\
        "<NewProtocol>%s</NewProtocol>\r\n"        \
        "<NewInternalPort>%u</NewInternalPort>\r\n"\
        "<NewInternalClient>%s"                    \
        "</NewInternalClient>\r\n"                 \
        "<NewEnabled>1</NewEnabled>\r\n"           \
        "<NewPortMappingDescription>%s"            \
        "</NewPortMappingDescription>\r\n"         \
        "<NewLeaseDuration>%d"                     \
        "</NewLeaseDuration>\r\n"

#define UPNP_DEL_PORT_MAPPING_PARAMS "<NewRemoteHost></NewRemoteHost>\r\n" \
        "<NewExternalPort>%u"                 \
        "</NewExternalPort>\r\n"              \
        "<NewProtocol>%s</NewProtocol>\r\n"        \


#if 1
#define UPNP_GET_GEN_PARAMS          "<NewPortMappingIndex>%d</NewPortMappingIndex>"
#else
#define UPNP_GET_GEN_PARAMS          "<NewPortMappingIndex>%i</NewPortMappingIndex>" \
        "<NewRemoteHost></NewRemoteHost>\r\n"      \
        "<NewExternalPort></NewExternalPort>\r\n"\
        "<NewProtocol></NewProtocol>\r\n"        \
        "<NewInternalPort></NewInternalPort>\r\n"\
        "<NewInternalClient>"                    \
        "</NewInternalClient>\r\n"                 \
        "<NewEnabled>1</NewEnabled>\r\n"           \
        "<NewPortMappingDescription>"              \
        "</NewPortMappingDescription>\r\n"         \
        "<NewLeaseDuration>"                       \
        "</NewLeaseDuration>\r\n"
#endif

#define UPNP_ACTION_ADD   "AddPortMapping"
#define UPNP_ACTION_DEL   "DeletePortMapping"
#define UPNP_ACTION_GET_SPE "GetSpecificPortMappingEntry"
#define UPNP_ACTION_GET_GEN "GetGenericPortMappingEntry"

#define UPNP_MAPPING_INFO "%s/%s[%s]-->%s:%s\r\n"
