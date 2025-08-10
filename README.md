# Arduino UNO R4 WiFi HttpClient Wrapper

An Arduino UNO R4 WiFi HttpClient wrapper with functionality of only GET & POST tailored for our thesis at *Mindanao State University - Iligan Institute of Technology (MSU-IIT)*.

# Installation
To install this library:

1. Go to Arduino IDE
2. `Sketch -> Include Library -> Manage Libraries...`
3. OR just `CTRL + SHIFT + i`.
4. Search the library name `R4HttpClient`, and ensure to install the latest version.

__OR__
1. Go to `<> Code ↓` near Add File in GitHub, then __Download as ZIP__.
2. Go to Arduino IDE, `Sketch -> Include Library -> Add .ZIP Library`.
3. Locate the library file downloaded and select it.

If you want to utilize this Library. Include the header file in your Arduino Sketch.
```cpp
#include <R4HttpClient.h>
```

# R4HttpClient - Functions:

`void begin(client, String url)`: Initializes the SSL client and extracts the host and endpoint from the provided URL for subsequent requests.

`void begin(client, String url, uint16_t port)`: Same as above, but also initializes a custom port for the client connection.

`void addHeader(String content)`: Adds custom headers to be sent with the HTTP request.

`void setTimeout(int ms)`: Sets a timeout for the client to terminate a request if it takes longer than the specified time in milliseconds.

`void setDebug(bool)`: Enables or disables debug mode, which displays raw HTTP messages.

`int POST(String requestBody)`: Sends a POST request to the server with the provided body content.

`int GET()`: Sends a GET request to the server without a request body, expecting a response.

`String getBody()`: Returns the body of the HTTP response after a request has been made.

`void close()`: Closes the connection to the server to free up resources.

# Example
> __NOTES__:<br>
> Initialization of the WiFiSSLClient object is required before to specify client.
> <br>
> > - Note that `WiFiSSLClient client;` should always be declared globally.
> > - `R4HttpClient http;` can be declared globally or locally.

> To use the debug mode, put right after the http.begin() function
> ```cpp
> http.setDebug(true);

__Example 1: Arduino UNO R4 WiFi GET Request__
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
  while (!Serial);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    Serial.println(F("Please upgrade the firmware"));

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println(F("Communication with WiFi module failed!"));
    while (true);
  }

  WiFi.begin(_SSID, _PASS);
  Serial.print(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("Successfully connected to WiFi!"));
  delay(1000); // give some time for the connection to stabilize

  http.begin(client, "https://icanhazdadjoke.com/slack", 443);
  http.setTimeout(3000);
  http.addHeader("User-Agent: Arduino UNO R4 WiFi");
  http.addHeader("Connection: close");

  int responseNum = http.GET();
  if (responseNum > 0) // OR if (responseNum == HTTP_CODE_OK) // 200 OK
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

__Example 2: Arduino UNO R4 WiFi POST Request with ArduinoJson__
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
  while (!Serial);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    Serial.println(F("Please upgrade the firmware"));

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println(F("Communication with WiFi module failed!"));
    while (true);
  }

  WiFi.begin(_SSID, _PASS);
  Serial.print(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("Successfully connected to WiFi!"));
  delay(1000); // give some time for the connection to stabilize

  StaticJsonDocument<512> doc;
  doc["example"] = "content";

  String requestBody;
  serializeJson(doc, requestBody);

  http.begin(client, "https://example.org", 443);
  http.setTimeout(3000);
  http.addHeader("User-Agent: Arduino UNO R4 WiFi");
  http.addHeader("Content-Type: application/json");

  int responseNum = http.POST(requestBody);
  if (responseNum > 0) // OR if (responseNum == HTTP_CODE_OK) // 200 OK
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

## License
This project is licensed under the GNU General Public License v3.0 (GPL-3.0). This license ensures that the software remains free and open-source, granting users the freedom to use, modify, and distribute the software. Any modifications or derivative works must also be licensed under the same terms, ensuring that the same freedoms are preserved in all copies and versions.

For more details, see the GNU General Public License v3.0.
[GNU Public License](https://www.gnu.org/licenses/gpl-3.0.html)
