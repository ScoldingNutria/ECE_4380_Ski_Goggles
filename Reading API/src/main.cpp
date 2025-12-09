// Squad Ski Listener RTOS-style
// Pulls all /active riders (read-only, no POST)
// Structured like RTOS 4 final

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

typedef struct task
{
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;


const unsigned long periodGCD = 100;
const unsigned long periodWiFi = 1000;
const unsigned long periodAPI = 5000;
const unsigned char numTasks = 2;

const char *WIFI_SSID = "NikoiPhone";
const char *WIFI_PASSWORD = "nikoswifi";
const char *BASE_URL = "https://squad-ski-api.onrender.com";

task tasks[numTasks];

unsigned long lastTick = 0;

int  globalCount = 0;
char globalFirstName[32] = {0};

String BuildURL(const char *path)
{
    String u = String(BASE_URL);
    if (!u.endsWith("/") && path[0] != '/')
    {
        u += "/";
    }
    u += path;
    return u;
}

void WiFi_Connect()
{
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
    {
        delay(200);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\n[WiFi] Connected");
        Serial.print("[WiFi] IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("\n[WiFi] FAILED");
    }
}

void PrintActiveRiders(JsonArray arr)
{
    globalCount = 0;
    bool stored = false;

    Serial.println("\n~~~~~~~~~ ACTIVE RIDERS ~~~~~~~~~");

    if (arr.size() == 0)
    {
        Serial.println("  (none online)");
    }

    for (JsonObject obj : arr)
    {
        const char *name  = obj["name"]    | "anon";
        const char *id    = obj["user_id"] | "";
        double lat        = obj["lat"]     | 0.0;
        double lon        = obj["lon"]     | 0.0;
        double alt        = obj["alt"]     | 0.0;
        const char *trail = obj["trail"]   | "";

        globalCount++;

        if (!stored)
        {
            strncpy(globalFirstName, name, sizeof(globalFirstName) - 1);
            globalFirstName[31] = '\0';
            stored = true;
        }

        Serial.printf("• %s (%s)\n", name, String(id).substring(0, 6).c_str());
        Serial.printf("    lat    : %.6f\n", lat);
        Serial.printf("    lon    : %.6f\n", lon);
        Serial.printf("    alt    : %.1f m\n", alt);
        Serial.printf("    trail  : '%s'\n\n", trail);
    }

    Serial.printf("[status] Active riders: %d\n", globalCount);
    if (globalCount > 0)
    {
        Serial.printf("[status] First rider : %s\n", globalFirstName);
    }

    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}


enum WiFiStates { WIFI_START, WIFI_CHECK };
int TickFct_WiFi(int state)
{
    switch (state)
    {
        case WIFI_START:
            WiFi_Connect();
            state = WIFI_CHECK;
            break;

        case WIFI_CHECK:
            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("[WiFi] Lost connection, reconnecting...");
                WiFi_Connect();
            }
            break;
    }

    return state;
}


enum APIStates { API_IDLE };
int TickFct_API(int state)
{
    switch (state)
    {
        case API_IDLE:
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                Serial.println("[API] No WiFi; skipping poll.");
                break;
            }

            HTTPClient http;
            http.begin(BuildURL("active"));

            int code = http.GET();
            if (code != 200)
            {
                Serial.print("[API] GET error: ");
                Serial.println(code);
                http.end();
                break;
            }

            String payload = http.getString();
            http.end();

            StaticJsonDocument<4096> doc;
            auto err = deserializeJson(doc, payload);

            if (err)
            {
                Serial.print("[API] JSON error: ");
                Serial.println(err.c_str());
                break;
            }

            PrintActiveRiders(doc.as<JsonArray>());
        }
        break;
    }

    return state;
}


void Tasks_Init()
{
    tasks[0].state       = WIFI_START;
    tasks[0].period      = periodWiFi;
    tasks[0].elapsedTime = 0;
    tasks[0].TickFct     = &TickFct_WiFi;

    tasks[1].state       = API_IDLE;
    tasks[1].period      = periodAPI;
    tasks[1].elapsedTime = 0;
    tasks[1].TickFct     = &TickFct_API;
}

void setup()
{
    Serial.begin(115200);
    delay(800);

    Tasks_Init();
    lastTick = millis();
}

void loop()
{
    unsigned long now = millis();

    if (now - lastTick >= periodGCD)
    {
        lastTick += periodGCD;

        for (int i = 0; i < numTasks; i++)
        {
            if (tasks[i].elapsedTime >= tasks[i].period)
            {
                tasks[i].state = tasks[i].TickFct(tasks[i].state);
                tasks[i].elapsedTime = 0;
            }
            tasks[i].elapsedTime += periodGCD;
        }
    }
}