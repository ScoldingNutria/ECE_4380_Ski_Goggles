#include "skills.h"


float IMU_heading = 0;
float IMU_roll    = 0;
float IMU_pitch   = 0;
float IMU_gforce  = 0;
float IMU_gforce_max = 0;



const char* DYN_HEADING_LABEL = "N";  
const char* DYN_TRAIL_LABEL   = "Unknown";

static float normHeading(float h) {
    while (h < 0)       h += 360.0f;
    while (h >= 360.0f) h -= 360.0f;
    return h;
}

const char* headingToDir16(float headingDeg) {
    static const char* dirs[16] = {
        " N ", " Ne", "NE", "nE ",
        " E ", " Es", "ES", "eS ",
        " S ", " Sw", "SW", "sW ",
        " W ", " Wn", "WN", "wN "
    };

    float h = normHeading(headingDeg);
    int idx = int((h + 11.25f) / 22.5f) % 16;
    return dirs[idx];
}


const char* mapHeading() {
    DYN_HEADING_LABEL = headingToDir16(IMU_heading);
    return DYN_HEADING_LABEL;
}


float TEMP_celsius = 0;
float TEMP_F = 0;


float GPS_lat       = 0;
float GPS_lon       = 0;
float GPS_alt       = 0;
float GPS_speed_mph = 0;
int   GPS_sats      = 0;


MenuItem menuItems[] = {
    {"  ",         &IMU_roll,      "   ", nullptr}, // 0 
    {"Menu",       &IMU_roll,      "",    nullptr}, // 1
    {"Options",    &IMU_roll,      "",    nullptr}, // 2
    {"Selected",     &IMU_roll,      "",    nullptr}, // 3
    {"Satellites", (float*)&GPS_sats, "", nullptr}, // 4
    {"Speed",      &GPS_speed_mph, "mph", nullptr}, // 5
    {"Roll",       &IMU_roll,      "",    nullptr}, // 6
    {"Heading",    nullptr,   "",   &DYN_HEADING_LABEL}, // 7 
    {"Pitch",      &IMU_pitch,     "",nullptr}, // 8
    {"G-Force",    &IMU_gforce_max,    "g",  nullptr}, // 9
    {"Temp",       &TEMP_F,  "F",  nullptr}, // 10
    {"Altitude",   &GPS_alt,       "ft", nullptr}, // 11
    {"Latitude",   &GPS_lat,       "",   nullptr}, // 12
    {"Longitude",  &GPS_lon,       "",   nullptr}, // 13
    {"Trail", nullptr, "", &DYN_TRAIL_LABEL}, // 14
    {"Speed",      &GPS_speed_mph, "mph", nullptr}  // 15
};

const int MENU_COUNT = sizeof(menuItems) / sizeof(menuItems[0]);


