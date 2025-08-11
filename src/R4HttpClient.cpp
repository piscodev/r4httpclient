#include "R4HttpClient.h"

R4HttpClient::R4HttpClient() : port(0), body(""), isDebug(false), client(nullptr) {}

R4HttpClient::~R4HttpClient()
{
  //DEBUG(F("R4HttpClient objects destroyed~"));
  if (client)
  {
    this->client->flush();
    this->client->stop();
  }
  this->headers.clear();
  this->port = 0;
  this->body = "";
  this->isDebug = false;
}

/*
 * @params: sslClient, url
 * @Description: Initializes client, url. Then, extracts the url to get the host, then the endpoint in which where it will do the post request
 */
void R4HttpClient::begin(WiFiSSLClient &sslClient, const String &url)
{
  this->client = &sslClient;
  this->setPort((url.startsWith("https://")) ? 443 : 80);
  this->extractUrlComponents(url);
}

/*
 * @params: sslClient, url, nport
 * @Description: Initializes client, url, port. Then, extracts the url to get the host, then the endpoint in which where it will do the post request
 */
void R4HttpClient::begin(WiFiSSLClient &sslClient, const String &url, const uint16_t &nport)
{
  this->client = &sslClient;
  this->setPort(nport);
  this->extractUrlComponents(url);
}

/*
 * @params: url
 * @Description: Parses the URL to extract the host and endpoint
 */
void R4HttpClient::extractUrlComponents(const String &url)
{
  int index = url.indexOf("://");
  if (index < 0)
  {
    //DEBUG(F("Invalid URL format"));
    Serial.println(F("Invalid URL format"));
    return;
  }

  index += 3; // move index past "://"
  int endpointIndex = url.indexOf("/", index);
  this->host = url.substring(index, (endpointIndex == -1 ? url.length() : endpointIndex));
  this->endpoint = (endpointIndex == -1) ? "/" : url.substring(endpointIndex);
}

void R4HttpClient::setPort(const uint16_t &nport)
{
  this->port = nport;
}

/*
 * @Description: Sets debug to see raw requests from the request
 * @params: debug
 */
void R4HttpClient::setDebug(const bool &debug)
{
  this->isDebug = debug;
}

/*
 * @params: content
 * @Description: Initializes the Content Headers to make the request method
 */
void R4HttpClient::addHeader(const String &content)
{
  this->headers.emplace_back(content);
}

/*
 * @params: ms
 * @Description: set timeout when request does not respond within specific time.
 */
void R4HttpClient::setTimeout(const int &ms)
{
  this->client->setTimeout(ms);
}

/*
 * @Description: Sends an HTTP request to the server using the specified method ("GET", "POST") and optional request body.
 * It first attempts to establish a connection to the server via SSL. If the connection fails, the function returns an error code.
 * Once connected, the request is assembled, including the HTTP method, endpoint, host, and headers, followed by the request body if provided.
 * After sending the request, the function reads the server's response, handles chunked transfer encoding for the response body,
 * and processes the HTTP status code. The function returns either the status code of the response or an appropriate error code if the request fails.
 *
 * @param method: The HTTP method for the request, such as "GET", "POST", etc.
 * @param requestBody: The body of the HTTP request, typically used for POST or PUT requests.
 * @return int Returns the HTTP status code from the server, or a custom error code if the connection or request fails.
 */
int R4HttpClient::sendRequest(const String &method, const String &requestBody)
{
  // any previous connection is closed
  if (this->client->connected())
    this->client->stop();

  unsigned long startAttemptTime = millis();
  while (!this->client->connect(this->host.c_str(), this->port)) // retry connect until timeout
  {
    if (millis() - startAttemptTime > this->client->getTimeout())
      return R4HTTP_ERROR_CONNECTION_REFUSED;

    delay(10); // short backoff
  }

  // manual prints
  this->client->print(method);
  this->client->print(" ");
  this->client->print(endpoint);
  this->client->print(" HTTP/1.1\r\n");
  this->client->print("Host: ");
  this->client->print(host);
  this->client->print("\r\n");
  for (const String &header : headers)
  {
    this->client->print(header);
    this->client->print("\r\n");
  }

  if (!requestBody.isEmpty())
  {
    this->client->print("Content-Length: ");
    this->client->print(requestBody.length());
    this->client->print("\r\n\r\n");
    this->client->print(requestBody);
  } else this->client->print("\r\n");

  // start reading response
  int statusCode = -1;
  bool isChunked = false;
  int contentLength = -1;
  this->body = "";

  ResponseState state = READ_HEADERS;
  while (this->client->connected() && state != END_RESPONSE)
  {
    if (state == READ_HEADERS)
    {
      String line = this->client->readStringUntil('\n');
      if (isDebug && (line.length() > 0))
        Serial.println(line);

      if (line.startsWith("HTTP/1.1 ")) // Parse status code
        statusCode = line.substring(9, 12).toInt();

      if (line.startsWith("Transfer-Encoding: chunked"))
        isChunked = true;

      if (line.startsWith("Content-Length: "))
        contentLength = line.substring(16).toInt();

      if (line == "\r") // end of headers
      {
        state = READ_BODY;
        this->headers.clear();
      }
    } else if (state == READ_BODY)
    {
      if (isChunked)
      {
        while (true)
        {
          String chunkSizeStr = this->client->readStringUntil('\n');
          int chunkSize = strtol(chunkSizeStr.c_str(), NULL, 16);
          if (chunkSize <= 0)
            break; // end of chunks

          while (chunkSize--)
          {
            if (this->client->available())
            {
              char c = this->client->read();
              this->body += c;
            }
          }
          this->client->read(); // \r
          this->client->read(); // \n
        }
      } else if (contentLength > 0)
      {
        int bytesRead = 0;
        while (bytesRead < contentLength)
        {
          if (this->client->available())
          {
            char c = this->client->read();
            this->body += c;
            bytesRead++;
          }
        }
      } else {

        // no content length, read until closed
        while (this->client->available())
        {
          char c = this->client->read();
          this->body += c;
        }
      }

      state = END_RESPONSE;
    }
  }

  return (statusCode == -1) ? R4HTTP_ERROR_NO_HTTP_SERVER : statusCode;
}

/*
 * @params: requestBody
 * @Description: called by reference base on parameter and centralized the method to send post request
 */
int R4HttpClient::POST(const String &requestBody)
{
  return this->sendRequest("POST", requestBody);
}

/*
 * @Description: centralized the method to send get request, empty request indicates the method only needs a response
 */
int R4HttpClient::GET()
{
  return this->sendRequest("GET", "");
}

/*
 * @Description: Initialized through client read() to get the response body after request
 */
String R4HttpClient::getBody() const
{
  return this->body;
}

/*
 * @Description: Close connection to prevent resource leaks
 */
void R4HttpClient::close()
{
  if (client)
    this->client->stop();
}
