#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "skills.h"
#include "trail_map.h"

#define GPS_RX 10
#define GPS_TX 9

static HardwareSerial GPSSerial(1);
static TinyGPSPlus gps;
static bool gps_online = false;

void gps_init() {
    GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
    gps_online = true;
    Serial.println("GPS ONLINE");
}

void gps_read_once() {
    if (!gps_online) return;

    while (GPSSerial.available())
        gps.encode(GPSSerial.read());

    if (gps.location.isValid()) {
        GPS_lat = gps.location.lat();
        GPS_lon = gps.location.lng();
    }
    if (gps.altitude.isValid()) GPS_alt = gps.altitude.meters();
    if (gps.speed.isValid()) GPS_speed_mph = gps.speed.mph();
    GPS_sats = gps.satellites.value();

    Serial.printf("GPS: %.6f, %.6f | %.1f mph | sats=%d\n",
                  GPS_lat, GPS_lon, GPS_speed_mph, GPS_sats);
}



const char* identify_trail() {

    double bestDist = 1e12; // huge number
    const char *bestTrail = "Unknown";

    for (int i = 0; i < TRAIL_POINT_COUNT; i++) {

        double dLat = TRAIL_POINTS[i].lat - GPS_lat;
        double dLon = TRAIL_POINTS[i].lon - GPS_lon;
        double distSq = dLat * dLat + dLon * dLon;

        if (distSq < bestDist) {
            bestDist = distSq;
            bestTrail = TRAIL_POINTS[i].name;
        }
    }

    
    return bestTrail;
}