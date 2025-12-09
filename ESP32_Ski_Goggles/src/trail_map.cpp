#include "trail_map.h"
#include <math.h>


const char* identify_trail(float curLat, float curLon) {
    double bestDist = 1e12;
    const char* bestTrail = "Unknown";

    for (int i = 0; i < TRAIL_POINT_COUNT; i++) {
        double dLat = TRAIL_POINTS[i].lat - curLat;
        double dLon = TRAIL_POINTS[i].lon - curLon;
        double distSq = dLat * dLat + dLon * dLon;

        if (distSq < bestDist) {
            bestDist = distSq;
            bestTrail = TRAIL_POINTS[i].name;
        }
    }

    return bestTrail;
}
