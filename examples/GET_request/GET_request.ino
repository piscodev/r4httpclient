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
  http.setTimeout(5000);
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
