#pragma once
#include <Arduino.h>

// ==== IMU DATA ====
extern float IMU_heading;
extern float IMU_roll;
extern float IMU_pitch;
extern float IMU_gforce;
extern String USER_ID;
extern const char* USER_NAME;

void imu_init();
void imu_read_once();

// ==== TEMP DATA ====
extern float TEMP_celsius;
extern float TEMP_F;

void temp_init();
void temp_read_once();

// ==== GPS DATA ====
extern float GPS_lat;
extern float GPS_lon;
extern float GPS_alt;
extern float GPS_speed_mph;
extern int   GPS_sats;
extern float IMU_gforce_max;
extern String TrailName;
extern const char *MY_USER_ID;

void gps_init();
void gps_read_once();

// ==== OLED DISPLAY ====
void oled_init();
void oled_show_speed(float speed_mph);
void oled_show_two_lines(const char* lineTop, const char* lineBottom);
void oled_show_two_lines(const char* lineTop, float lineBottom);
void oled_show_two_lines(float lineTop, const char* lineBottom);
void oled_show_two_lines(float lineTop, float lineBottom);
void oled_show_three_lines(const char* lineTop, const char* lineMid, const char* lineBottom);
void oled_show_three_lines(float lineTop, float lineMid, float lineBottom);
bool api_pushTelemetry(const char *userId,
                       float GPS_lat,
                       float GPS_lon,
                       const String &trail);

bool api_pullActive();

// ==== MENU STRUCT ====
typedef struct {
    const char*  label;
    const float* data;
    const char*  data_label;
    const char** dyn_label;
} MenuItem;

extern MenuItem menuItems[];
extern const int MENU_COUNT;

const char* headingToDir16(float headingDeg);
const char* mapHeading();

extern const char* DYN_TRAIL_LABEL; 





void network_fsm();


void register_user();
void update_status(bool active);
void get_active_users();



static const char *API_BASE_URL = "https://squad-ski-api.onrender.com";


inline String api_buildURL(const char *path)
{
    String u = String(API_BASE_URL);
    if (!u.endsWith("/") && path[0] != '/')
        u += "/";
    u += path;
    return u;
}

