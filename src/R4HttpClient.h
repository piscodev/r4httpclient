#ifndef RFExpress_CLIENT_H
#define RFExpress_CLIENT_H

#include <WiFiS3.h>

class RFExpressClient
{
  private:
    int port;
    String host;
    String endpoint;
    String body;
    WiFiSSLClient client;
    std::vector<String> headers;

    void extractUrlComponents(const String &url);
    int sendRequest(const String &method, const String &requestBody);
    int readResponse();

    // disable copy constructor and assignment operator
    RFExpressClient(const RFExpressClient&) = delete;
    RFExpressClient& operator=(const RFExpressClient&) = delete;

  public:
    RFExpressClient();
    ~RFExpressClient();

    void begin(const WiFiSSLClient &sslClient, const String &url);
    void begin(const WiFiSSLClient &sslClient, const String &url, const int &nport);
    void addHeader(const String &content);
    void setTimeout(const int &ms);
    int POST(const String &requestBody);
    int GET();
    String getBody() const;
    void close();
};

#endif  //RFExpress_CLIENT_H
