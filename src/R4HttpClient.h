#ifndef R4HTTP_CLIENT_H
#define R4HTTP_CLIENT_H

#include <WiFiS3.h>

class R4HttpClient
{
  private:
    WiFiSSLClient client;
    String host;
    String endpoint;
    int port;
    std::vector<String> headers;
    String body;

    static const int initialBufferSize = 512;
    char *buffer;
    int bufferSize;
    int bufferIndex;

    void resizeBuffer(int newSize); // Method to resize the buffer

    // Disable copy constructor and assignment operator // reducing cost
    R4HttpClient(const R4HttpClient&) = delete;
    R4HttpClient& operator=(const R4HttpClient&) = delete;

  public:
    R4HttpClient();
    ~R4HttpClient();

    void begin(const WiFiSSLClient &sslClient, const String &url, const int& nport);
    void setHeader(const String &content);
    void setTimeout(const int &ms);
    int POST(const String &requestBody);
    int GET();
    String getBody() const;
    void close();
};

#endif  //R4HTTP_CLIENT_H
