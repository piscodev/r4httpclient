#ifndef R4HttpClient_CLIENT_H
#define R4HttpClient_CLIENT_H

#include <WiFiS3.h>

class R4HttpClient
{
  private:
    int port;
    String host;
    String endpoint;
    String body;
    WiFiSSLClient client;
    std::vector<String> headers;

    void extractUrlComponents(const String &url);
    void setPort(const int &nport);
    int sendRequest(const String &method, const String &requestBody);
    int readResponse();

    // disable copy constructor and assignment operator
    R4HttpClient(const R4HttpClient&) = delete;
    R4HttpClient& operator=(const R4HttpClient&) = delete;

  public:
    R4HttpClient();
    ~R4HttpClient();

    void begin(const WiFiSSLClient &sslClient, const String &url);
    void begin(const WiFiSSLClient &sslClient, const String &url, const int &nport);
    void addHeader(const String &content);
    void setTimeout(const int &ms);
    int POST(const String &requestBody);
    int GET();
    String getBody() const;
    void close();
};

#endif  //R4HttpClient_CLIENT_H
