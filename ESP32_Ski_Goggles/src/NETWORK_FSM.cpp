#include <Arduino.h>
#include <HTTPClient.h>
#include "skills.h"
#include "network_fsm.h"
#include "wifi.h"




enum net_states {
    API_init,
    NET_write,   
    NET_read     
};

static net_states net_state = NET_write;


static unsigned long write_cnt = 0;
static unsigned long read_cnt  = 0;


static const unsigned long WRITE_PERIOD = 2000;   
static const unsigned long READ_PERIOD  = 2000;   

String TrailName = "Test_Trail";
const char* MY_USER_ID = "12f5b4e2-c719-4f91-8032-5b6d58c63577";


    
void network_fsm() {
    static net_states net_state = API_init;
    static unsigned long write_cnt = 0;
    static unsigned long read_cnt  = 0;

    static unsigned long last_tick = 0;
    if (millis() - last_tick < 5) return;  
    last_tick = millis();

    switch (net_state) {

        case API_init:
            Serial.println("registering user...");
            net_state = NET_write;
           
            break;

        case NET_write:
            TrailName = DYN_TRAIL_LABEL;
            api_pushTelemetry(
                MY_USER_ID,
                GPS_lat,
                GPS_lon,
                TrailName
            );
            Serial.println("telemetry pushed...");
            net_state = NET_read;
            break;

        case NET_read:

            net_state = NET_write;
            break;
    }
}