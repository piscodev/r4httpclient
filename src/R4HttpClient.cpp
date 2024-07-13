#include "R4HttpClient.h"

R4HttpClient::R4HttpClient() : port(0), body(""), headers(), buffer(nullptr), bufferSize(0), bufferIndex(0)
{
    // Allocate initial buffer
    resizeBuffer(initialBufferSize);
}

R4HttpClient::~R4HttpClient()
{
    // Deallocate buffer
    if (buffer != nullptr)
    {
        delete[] buffer;
        buffer = nullptr;
    }
    this->client.flush();
    this->close();
}

/*
 * @params: sslClient, url, nport
 * @Description: Initializes client, url, port. Then, extracts the url to get the host, then the endpoint in which where it will do the post request
 */
void R4HttpClient::begin(const WiFiSSLClient &sslClient, const String &url, const int& nport)
{
    this->client = sslClient;
    this->port = nport;

    // extract the host and endpoint from the URL
    int index;
    if (url.startsWith("https://"))
        index = 8; // length of "https://"
    else if (url.startsWith("http://"))
        index = 7; // length of "http://"
    else {
        // handle the case where the URL does not start with "http://" or "https://"
        Serial.println("Invalid URL format");
        return;
    }
    int endpointIndex = url.indexOf("/", index);

    this->host = url.substring(index, endpointIndex);
    this->endpoint = url.substring(endpointIndex);
}

/*
 * @params: content
 * @Description: Initializes the Content Headers to make the request connection.
 */
void R4HttpClient::setHeader(const String &content)
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

/*
 * @params: requestBody
 * @Description: called by reference base on declared data to post request, then wait for response, then if responded, break, then init response body
 * then clear content headers to prevent additional headers from previous request.
 */
int R4HttpClient::POST(const String &requestBody)
{
    if (!this->client.connect(this->host.c_str(), this->port))
        return -1;

    this->client.println("POST " + endpoint + " HTTP/1.1");
    this->client.println("Host: " + host);
    for (const String& header : headers)
        this->client.println(header);
    this->client.println("Content-Length: " + String(requestBody.length()));
    this->client.println();
    this->client.print(requestBody);

    // wait for the server response
    int responseCode = -1;
    while (this->client.connected())
    {
        String line = this->client.readStringUntil('\n');
        if (line.startsWith("HTTP/1.1 "))
            responseCode = line.substring(9, 12).toInt(); // response code

        if (line == "\r")
            break;
    }
    this->headers.clear(); // clear previous headers

    // Read and process the chunked response
    String body = "";
    while (true)
    {
        // Read chunk size
        String chunkSizeStr = this->client.readStringUntil('\n');
        int chunkSize = strtol(chunkSizeStr.c_str(), NULL, 16);
        if (chunkSize == 0)
            break; // End of chunks

        // Read chunk data
        for (int i = 0; i < chunkSize; i++)
            body += (char)this->client.read();

        // Read the trailing \r\n after the chunk
        this->client.readStringUntil('\n');
    }
    this->body = body;

    return responseCode;
}

int R4HttpClient::GET()
{
    if (!this->client.connect(this->host.c_str(), this->port))
        return -1;

    this->client.println("GET " + this->endpoint + " HTTP/1.1");
    this->client.print("Host: ");
    this->client.println(this->host);
    for (const String& header : headers)
        this->client.println(header);
    this->client.println();

    int responseCode = -1;
    bool headersEnd = false;
    bool chunkedEncoding = false;
    int contentLength = 0;

    while (this->client.connected())
    {
        String line = this->client.readStringUntil('\n');
        if (line.startsWith("HTTP/1.1 "))
            responseCode = line.substring(9, 12).toInt();

        if (line.startsWith("Content-Length: "))
            contentLength = line.substring(16).toInt();

        if (line == "\r")
        {
            headersEnd = true;
            break;
        }
    }

    if (headersEnd)
    {
        // Read and process the response body
        String body = "";
        if (contentLength > 0)
        {
            for (int i = 0; i < contentLength; i++)
                body += (char)this->client.read();
        } else {

            // Read chunked response
            while (true)
            {
                String chunkSizeStr = this->client.readStringUntil('\n');
                int chunkSize = strtol(chunkSizeStr.c_str(), NULL, 16);
                if (chunkSize == 0)
                    break; // End of chunks

                for (int i = 0; i < chunkSize; i++)
                    body += (char)this->client.read();

                // Read the trailing \r\n after the chunk
                this->client.readStringUntil('\n');
            }
        }
    }
    this->body = body;
  }
  this->headers.clear();

  return responseCode;
}

void R4HttpClient::resizeBuffer(int newSize)
{
    if (newSize <= 0)
        return; // Sanity check

    char *newBuffer = new char[newSize];
    if (newBuffer == nullptr)
    {
        // Failed to allocate new buffer, handle error
        Serial.println("Failed to allocate memory for buffer.");
        return;
    }

    // Copy existing data to new buffer
    if (buffer != nullptr)
    {
        memcpy(newBuffer, buffer, bufferIndex);
        delete[] buffer;
    }

    buffer = newBuffer;
    bufferSize = newSize;
}

/*
 * @Description: Initialized through client.readStringUntil('\n'); to get the response body after request
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
