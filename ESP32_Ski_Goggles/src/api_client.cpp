
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "skills.h"

bool api_pushTelemetry(const char *userId,
                       float GPS_lat,
                       float GPS_lon,
                       const String &trail)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("No WiFi");
        return false;
    }

    String trailTrim = trail.length() > 16 ? trail.substring(0,16) : trail;

    String url = api_buildURL("update");

    HTTPClient http;
    http.begin(url);
    http.addHeader("Content Type", "application/json");

    
    String body;
    body.reserve(200);
    body  = "{";
    body += "\"user_id\":\""; body += userId; body += "\",";
    body += "\"active\":true,";
    body += "\"lat\":"; body += String(GPS_lat, 6); body += ",";
    body += "\"lon\":"; body += String(GPS_lon, 6); body += ",";
    body += "\"trail\":\""; body += trailTrim; body += "\"";
    body += "}";

    Serial.println("API POST");
    Serial.println(body);

    int httpCode = http.POST(body);
    String resp = http.getString();
    http.end();

    Serial.printf("API Code: %d\n", httpCode);
    Serial.println("API Resp: " + resp);

    return (httpCode == 200);
}
