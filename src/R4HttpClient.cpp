#include "R4HttpClient.h"

R4HttpClient::R4HttpClient() : port(0), body("") {}

R4HttpClient::~R4HttpClient()
{
  this->client.flush();
  this->client.stop();
  this->headers.clear();
  this->port = 0;
  this->body = "";
}

/*
 * @params: sslClient, url
 * @Description: Initializes client, url. Then, extracts the url to get the host, then the endpoint in which where it will do the post request
 */
void R4HttpClient::begin(const WiFiSSLClient &sslClient, const String &url)
{
  this->client = sslClient;
  this->setPort((url.startsWith("https://")) ? 443 : 80);
  this->extractUrlComponents(url);
}

/*
 * @params: sslClient, url, nport
 * @Description: Initializes client, url, port. Then, extracts the url to get the host, then the endpoint in which where it will do the post request
 */
void R4HttpClient::begin(const WiFiSSLClient &sslClient, const String &url, const int &nport)
{
  this->client = sslClient;
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
    Serial.println(F("Invalid URL format"));
    return;
  }

  index += 3; // move index past "://"
  int endpointIndex = url.indexOf("/", index);
  this->host = url.substring(index, (endpointIndex == -1 ? url.length() : endpointIndex));
  this->endpoint = (endpointIndex == -1) ? "/" : url.substring(endpointIndex);
}

void R4HttpClient::setPort(const int &nport)
{
  this->port = nport;
}

/*
 * @params: content
 * @Description: Initializes the Content Headers to make the request method
 */
void R4HttpClient::addHeader(const String &content)
{
  this->headers.push_back(content);
}

/*
 * @params: ms
 * @Description: set timeout when request does not respond within specific time.
 */
void R4HttpClient::setTimeout(const int &ms)
{
  this->client.setTimeout(ms);
}

int R4HttpClient::sendRequest(const String &method, const String &requestBody)
{
  if (!this->client.connect(host.c_str(), port))
  {
    Serial.println(F("Connection failed"));
    return -1;
  }

  // intended for more faster execution
  client.print(method);
  client.print(" ");
  client.print(endpoint);
  client.print(" HTTP/1.1\r\n");
  client.print("Host: ");
  client.print(host);
  client.print("\r\n");

  for (const String &header : headers)
  {
    client.print(header);
    client.print("\r\n");
  }

  if (!requestBody.isEmpty())
  {
    client.print("Content-Length: ");
    client.print(requestBody.length());
    client.print("\r\n\r\n");
    client.print(requestBody);
  } else client.print("\r\n");

  return readResponse();
}

/*
 * @params: requestBody
 * @Description: called by reference base on parameter and centralized the method to send post request
 */
int R4HttpClient::POST(const String &requestBody)
{
  return sendRequest("POST", requestBody);
}

/*
 * @Description: centralized the method to send get request, empty request indicates the method only needs a reponse
 */
int R4HttpClient::GET()
{
  return sendRequest("GET", "");
}

/*
 * @Description: declared status code at -1, then checks if the client connection successfully connected then read response with status code
 *               then break when carriage return is found at the same line. Basically, each lines are iterated,
                 and when it prints new line and that line, and that row line is empty, then break. Clear headers.

                 Read Body: Check if data available from the client then read each chunk lines, then iterate to print for response body
                 while decrementing chunkSize for precise chunk response client read body
 */
int R4HttpClient::readResponse()
{
  int statusCode = -1;
  //this->body.reserve(512); // Pre-allocate memory for body

  // Read status line
  while (this->client.connected())
  {
    String line = this->client.readStringUntil('\n');
    if (line.startsWith("HTTP/1.1 "))
      statusCode = line.substring(9, 12).toInt();

    if (line == "\r")
      break;
  }
  this->headers.clear(); // clear headers after request

  // read body
  while (this->client.available())
  {
    String chunkSizeStr = this->client.readStringUntil('\n');
    int chunkSize = strtol(chunkSizeStr.c_str(), NULL, 16);
    if (chunkSize == 0)
      break; // end of chunks

    // read chunk directly into body
    while (chunkSize--)
      if (this->client.available())
        this->body += (char)this->client.read();

    this->client.readStringUntil('\n');
  }

  return statusCode;
}

/*
 * @Description: Initialized through client.read() to get the response body after request
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
  this->client.stop();
}
