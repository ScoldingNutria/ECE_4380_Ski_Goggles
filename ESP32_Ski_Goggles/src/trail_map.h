#pragma once


typedef struct {
    const char *name;
    double lat;
    double lon;
} TrailPoint;



static const TrailPoint TRAIL_POINTS[] = {
    {"Schoolmarm", 39.605310, -105.951932},
    {"Schoolmarm", 39.606002, -105.950880},
    {"Frenchman", 39.603120, -105.953441},
    {"Frenchman", 39.603871, -105.952332},
    {"Springdipper", 39.599472, -105.949663},
    {"ECE", 33.58670, -101.87533},
    
};



static const int TRAIL_POINT_COUNT =
    sizeof(TRAIL_POINTS) / sizeof(TRAIL_POINTS[0]);



#ifdef __cplusplus
extern "C" {
#endif

const char* identify_trail(float lat, float lon);

#ifdef __cplusplus
}
#endif
