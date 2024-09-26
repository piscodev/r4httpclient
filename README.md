# Arduino UNO R4 WiFi HttpClient Wrapper
- - - -
An Arduino UNO R4 WiFi HttpClient wrapper with functionality of only GET & POST tailored for our thesis at *Mindanao State University - Iligan Institute of Technology (MSU-IIT)*.

# R4HttpClient - Function Descriptions:
- - - -
**void begin(client, url)**: Initializes the SSL client and extracts the host and endpoint from the provided URL for subsequent requests.

**void begin(client, url, port)**: Same as above, but also initializes a custom port for the client connection.

**void addHeader(content)**: Adds custom headers to be sent with the HTTP request.

**void setTimeout(ms)**: Sets a timeout for the client to terminate a request if it takes longer than the specified time in milliseconds.

**int POST(String requestBody)**: Sends a POST request to the server with the provided body content.

**int GET()**: Sends a GET request to the server without a request body, expecting a response.

**String getBody()**: Returns the body of the HTTP response after a request has been made.

**void close()**: Closes the connection to the server to free up resources.

## License
- - - -
[GNU Public License](https://www.gnu.org/licenses/gpl-3.0.html)
