#ifndef R4HttpClient_CLIENT_H
#define R4HttpClient_CLIENT_H

// Testing purposes
//extern const bool R4HTTPCLIENT_DEBUG_MODE;
/*
 * @Description: The backslash \ in the macro definition is used to indicate line continuation in C/C++ preprocessor macros. (https://www.geeksforgeeks.org/macros-and-its-types-in-c-cpp/)
 *               Each line must end with a \ to tell the preprocessor that the macro definition continues on the next line, if not, the compiler reads it as new statement.
 *
 *               Without the do-while loop, it causes issue, such as inside conditional statements or multi-line expansions
 *
 * if (someCondition)
 *   if (condition)
 *     Serial.println("Debugging message");
 *   else
 *     foo();
 */
//#define DEBUG(x)                 \
//  do {                           \
//    if (R4HTTPCLIENT_DEBUG_MODE) \
//      Serial.println(x);         \
//  } while (0)

#include <WiFiS3.h>

// (https://github.com/espressif/arduino-esp32)
#define R4HTTP_ERROR_CONNECTION_REFUSED    (-1)
//#define R4HTTP_ERROR_SEND_HEADER_FAILED    (-2)
//#define R4HTTP_ERROR_SEND_PAYLOAD_FAILED   (-3)
#define R4HTTP_ERROR_NOT_CONNECTED         (-4)
#define R4HTTP_ERROR_CONNECTION_LOST       (-5)
//#define R4HTTP_ERROR_NO_STREAM             (-6)
#define R4HTTP_ERROR_NO_HTTP_SERVER        (-7)
//#define R4HTTP_ERROR_TOO_LESS_RAM          (-8) // couldn't find workround for renesas_uno architecture
//#define R4HTTP_ERROR_ENCODING              (-9)
//#define R4HTTP_ERROR_STREAM_WRITE         (-10)
//#define R4HTTP_ERROR_READ_TIMEOUT         (-11)

// HTTP codes see RFC7231
enum HttpStatus
{
  HTTP_CODE_CONTINUE = 100,
  HTTP_CODE_SWITCHING_PROTOCOLS = 101,
  HTTP_CODE_PROCESSING = 102,
  HTTP_CODE_OK = 200,
  HTTP_CODE_CREATED = 201,
  HTTP_CODE_ACCEPTED = 202,
  HTTP_CODE_NON_AUTHORITATIVE_INFORMATION = 203,
  HTTP_CODE_NO_CONTENT = 204,
  HTTP_CODE_RESET_CONTENT = 205,
  HTTP_CODE_PARTIAL_CONTENT = 206,
  HTTP_CODE_MULTI_STATUS = 207,
  HTTP_CODE_ALREADY_REPORTED = 208,
  HTTP_CODE_IM_USED = 226,
  HTTP_CODE_MULTIPLE_CHOICES = 300,
  HTTP_CODE_MOVED_PERMANENTLY = 301,
  HTTP_CODE_FOUND = 302,
  HTTP_CODE_SEE_OTHER = 303,
  HTTP_CODE_NOT_MODIFIED = 304,
  HTTP_CODE_USE_PROXY = 305,
  HTTP_CODE_TEMPORARY_REDIRECT = 307,
  HTTP_CODE_PERMANENT_REDIRECT = 308,
  HTTP_CODE_BAD_REQUEST = 400,
  HTTP_CODE_UNAUTHORIZED = 401,
  HTTP_CODE_PAYMENT_REQUIRED = 402,
  HTTP_CODE_FORBIDDEN = 403,
  HTTP_CODE_NOT_FOUND = 404,
  HTTP_CODE_METHOD_NOT_ALLOWED = 405,
  HTTP_CODE_NOT_ACCEPTABLE = 406,
  HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED = 407,
  HTTP_CODE_REQUEST_TIMEOUT = 408,
  HTTP_CODE_CONFLICT = 409,
  HTTP_CODE_GONE = 410,
  HTTP_CODE_LENGTH_REQUIRED = 411,
  HTTP_CODE_PRECONDITION_FAILED = 412,
  HTTP_CODE_PAYLOAD_TOO_LARGE = 413,
  HTTP_CODE_URI_TOO_LONG = 414,
  HTTP_CODE_UNSUPPORTED_MEDIA_TYPE = 415,
  HTTP_CODE_RANGE_NOT_SATISFIABLE = 416,
  HTTP_CODE_EXPECTATION_FAILED = 417,
  HTTP_CODE_MISDIRECTED_REQUEST = 421,
  HTTP_CODE_UNPROCESSABLE_ENTITY = 422,
  HTTP_CODE_LOCKED = 423,
  HTTP_CODE_FAILED_DEPENDENCY = 424,
  HTTP_CODE_UPGRADE_REQUIRED = 426,
  HTTP_CODE_PRECONDITION_REQUIRED = 428,
  HTTP_CODE_TOO_MANY_REQUESTS = 429,
  HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  HTTP_CODE_INTERNAL_SERVER_ERROR = 500,
  HTTP_CODE_NOT_IMPLEMENTED = 501,
  HTTP_CODE_BAD_GATEWAY = 502,
  HTTP_CODE_SERVICE_UNAVAILABLE = 503,
  HTTP_CODE_GATEWAY_TIMEOUT = 504,
  HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED = 505,
  HTTP_CODE_VARIANT_ALSO_NEGOTIATES = 506,
  HTTP_CODE_INSUFFICIENT_STORAGE = 507,
  HTTP_CODE_LOOP_DETECTED = 508,
  HTTP_CODE_NOT_EXTENDED = 510,
  HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED = 511
};

enum ResponseState
{
  READ_HEADERS,
  READ_BODY,
  END_RESPONSE
};

class R4HttpClient
{
  private:
    uint16_t port;
    String host;
    String endpoint;
    String body;
    bool isDebug;
    WiFiSSLClient *client;
    std::vector<String> headers;

    void extractUrlComponents(const String &url);
    void setPort(const uint16_t &nport);
    int sendRequest(const String &method, const String &requestBody);

    // disable copy constructor and assignment operator
    R4HttpClient(const R4HttpClient&) = delete;
    R4HttpClient& operator=(const R4HttpClient&) = delete;

  public:
    R4HttpClient();
    ~R4HttpClient();

    void begin(WiFiSSLClient &sslClient, const String &url);
    void begin(WiFiSSLClient &sslClient, const String &url, const uint16_t &nport);

    void addHeader(const String &content);
    void setTimeout(const int &ms);
    void setDebug(const bool &debug);
    int POST(const String &requestBody);
    int GET();
    String getBody() const;
    void close();
};

#endif  //R4HttpClient_CLIENT_H
