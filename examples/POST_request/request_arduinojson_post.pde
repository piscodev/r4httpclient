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
