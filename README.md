# Arduino UNO R4 WiFi HttpClient Wrapper

An Arduino UNO R4 WiFi HttpClient wrapper with functionality of only GET & POST tailored for our thesis at *Mindanao State University - Iligan Institute of Technology (MSU-IIT)*.

# Installation
To install this library:

1. Go to `<> Code ↓` near Add File in GitHub, then __Download as ZIP__.
2. Go to Arduino IDE, `Sketch -> Include Library -> Add .ZIP Library`.
3. Locate the library file downloaded and select it.

If you want to utilize this Library. Include the header file in your Arduino Sketch.
```cpp
#include <R4HttpClient.h>
```

# R4HttpClient - Function Descriptions:

__void begin(client, url)__: Initializes the SSL client and extracts the host and endpoint from the provided URL for subsequent requests.

__void begin(client, url, port)__: Same as above, but also initializes a custom port for the client connection.

__void addHeader(content)__: Adds custom headers to be sent with the HTTP request.

__void setTimeout(ms)__: Sets a timeout for the client to terminate a request if it takes longer than the specified time in milliseconds.

__int POST(String requestBody)__: Sends a POST request to the server with the provided body content.

__int GET()__: Sends a GET request to the server without a request body, expecting a response.

__String getBody()__: Returns the body of the HTTP response after a request has been made.

__void close()__: Closes the connection to the server to free up resources.

# Example
> __NOTES__:<br>
> Initialization of the WiFiSSLClient object is required before to specify client.
> <br><br>
> WiFiSSLClient client; <br>
> R4HttpClient http;
> >R4HttpClient and WiFiSSLClient objects can declared globally or locally.

__Example 1: Arduino UNO R4 WiFi POST Request with ArduinoJson__
```cpp
#include "R4WiFi_secrets.h"
#include <R4HttpClient.h>
#include <ArduinoJson.h>

WiFiSSLClient client;
R4HttpClient http;

const char* _SSID = SECRET_SSID;
const char* _PASS = SECRET_PASS;

void setup()
{
    Serial.begin(115200);

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
        Serial.println("Please upgrade the firmware");

    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");
        while (true);
    }

    WiFi.begin(_SSID, _PASS);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Successfully connected to WiFi!");

    StaticJsonDocument<512> doc;
    doc["example"] = "content";

    String requestBody;
    serializeJson(doc, requestBody);

    http.begin(client, "https://example.org", 443);
    http.setTimeout(3000);
    http.setHeader("User-Agent: Arduino UNO R4 WiFi");
    http.setHeader("Content-Type: application/json");

    int responseNum = http.POST(requestBody);
    if (responseNum > 0)
    {
        String responseBody = http.getBody();
        Serial.println(responseBody);
        Serial.println("Response code: " + String(responseNum));
    } else {
        Serial.println("Request Failed: " + String(responseNum));
    }

    http.close();
}

void loop()
{
}
```

__Example 2: Arduino UNO R4 WiFi GET Request__
```cpp
#include "R4WiFi_secrets.h"
#include <R4HttpClient.h>

WiFiSSLClient client;
R4HttpClient http;

const char* _SSID = SECRET_SSID;
const char* _PASS = SECRET_PASS;

void setup()
{
  Serial.begin(115200);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    Serial.println("Please upgrade the firmware");

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  WiFi.begin(_SSID, _PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Successfully connected to WiFi!");

  http.begin(client, "https://icanhazdadjoke.com/slack", 443);
  http.setTimeout(3000);
  http.setHeader("User-Agent: Arduino UNO R4 WiFi");
  http.setHeader("Connection: close");

  int responseNum = http.GET();
  if (responseNum > 0)
  {
    // Get body
    String responseBody = http.getBody();
    Serial.println(responseBody);
    Serial.println("Response code: " + String(responseNum));
  } else {
    Serial.println("Request Failed: " + String(responseNum));
  }

  http.close();
}

void loop()
{
}
```

## License

[GNU Public License](https://www.gnu.org/licenses/gpl-3.0.html)
