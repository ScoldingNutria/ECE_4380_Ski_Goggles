#include <Arduino.h>
#include "skills.h"
#include "ski_fsm.h"
#include "trail_map.h"


enum ski_states {
    INIT_fsm,
    read_imu,
    read_temp,
    read_gps,
    com_attiny,
    write_display,
    wait_tilt,
    wait_flat,
    menu_enter,
    menu_options,
    imu_watchdog
};

static ski_states state = INIT_fsm;
static unsigned short flat_cnt = 0;
static unsigned short tilt_cnt = 0;
static unsigned short delay_cnt = 0;

static int menuIndex = 0;
static int line_one_index   = 0;
static int line_two_index   = 0;
static int line_three_index = 0;

static int line_one_set = 7;
static int line_two_set = 7;
static int line_three_set = 7;
static int line_cnt = 0;

static int push_line_one = 0;
static int push_line_two = 0;
static int push_line_three = 0;

static int display_select = 0;

extern float IMU_roll;
extern float IMU_pitch;
extern float GPS_speed_mph;

void ski_fsm () {
    switch(state) {

        case INIT_fsm:
            //Serial.println("STATE: INIT_fsm");
            imu_init();
            temp_init();
            gps_init();
            oled_init();
            flat_cnt = 0;
            tilt_cnt = 0;
            delay_cnt = 0;
            state = read_imu;
            break;

        case read_imu:
            //Serial.println("STATE: read_imu");
            line_cnt = 0;
            imu_read_once();

            if (fabs(IMU_roll) > 20.0f) {
                if (flat_cnt < 65535) flat_cnt++;
            } else {
                flat_cnt = 0;
            }

            if (flat_cnt > 20) {
                state = wait_tilt;
            } else {
                state = read_temp;
            }
            break;

        case read_temp:
            //Serial.println("STATE: read_temp");
            temp_read_once();
            state = read_gps;
            break;

        case read_gps: {
            gps_read_once();
            const char* trail = identify_trail(GPS_lat, GPS_lon);
            DYN_TRAIL_LABEL = trail;

            state = com_attiny;
            break;
}


        case com_attiny:
            //Serial.println("STATE: com_attiny (stub)");
            state = write_display;
            break;

        
        case write_display: {
            mapHeading();

            //Serial.println("STATE: write_display");

            line_one_index = push_line_one;;
            line_two_index = push_line_two;
            line_three_index = push_line_three ;

            MenuItem* a = &menuItems[line_one_index];
            MenuItem* b = &menuItems[line_two_index];
            MenuItem* c = &menuItems[line_three_index];

            char L1[20];
            char L2[20];
            char L3[20];

            // oled line 1
            if (a->data) {
                snprintf(L1, sizeof(L1), "%.1f %s", *(a->data), a->data_label);
            } else {
                const char* labelA = (a->dyn_label != nullptr) ? *(a->dyn_label) : a->label;
                strncpy(L1, labelA, sizeof(L1)-1);
                L1[sizeof(L1)-1] = '\0';
            }

           // oled line 2
            if (b->data) {
                snprintf(L2, sizeof(L2), "%.1f %s", *(b->data), b->data_label);
            } else {
                const char* labelB = (b->dyn_label != nullptr) ? *(b->dyn_label) : b->label;
                strncpy(L2, labelB, sizeof(L2)-1);
                L2[sizeof(L2)-1] = '\0';
            }

            // oled line 3
            if (c->data) {
                snprintf(L3, sizeof(L3), "%.1f %s", *(c->data), c->data_label);
            } else {
                const char* labelC = (c->dyn_label != nullptr) ? *(c->dyn_label) : c->label;
                strncpy(L3, labelC, sizeof(L3)-1);
                L3[sizeof(L3)-1] = '\0';
            }

            oled_show_three_lines(L1, L2, L3);

            state = read_imu;
            break;
        }

        
        case wait_tilt:
            //Serial.println("STATE: wait_tilt");
            imu_read_once();

            if (IMU_roll > 30.0f) {
                state = menu_enter;
            } else if (IMU_roll < -30.0f) {
                flat_cnt = 0;
                state = read_imu;
            }
            break;

        case wait_flat:
            //Serial.println("STATE: wait_flat");
            imu_read_once();

            if (fabs(IMU_roll) < 5.0f) {
                
                state = menu_options;
            }

            oled_show_three_lines(
                menuItems[display_select].label,
                menuItems[3].label,
                menuItems[0].label
            );


            break;

        
        case menu_enter:
            //Serial.println("STATE: menu_enter");

            imu_read_once();

            line_one_set   = 5;
            line_two_set   = 5;
            line_three_set = 10;

            oled_show_three_lines(
                menuItems[1].label,
                menuItems[2].label,
                menuItems[0].label
            );

            if (IMU_roll < -30.0f) {
                flat_cnt = 0;
                state = read_imu;
            }

            if (IMU_roll > -30.0f)
                delay_cnt++;

            if (delay_cnt > 100)
                state = menu_options;

            IMU_gforce_max = 0;

            break;

        

        
        case menu_options:
            delay_cnt = 0;
            imu_read_once();
            Serial.println("STATE: menu_options");

            line_one_index   = line_one_set - 1;
            line_two_index   = line_one_set;
            line_three_index = line_one_set + 1;

            oled_show_three_lines(
                menuItems[line_one_index].label,
                menuItems[line_two_index].label,
                menuItems[line_three_index].label
            );

            if (IMU_roll < -30.0f) {
                flat_cnt = 0;
                state = read_imu;
            }

            if (IMU_roll > 30.0f && line_cnt == 0) {
                push_line_one = line_one_set;
                Serial.println("line one pushed");
                oled_show_three_lines(
                    menuItems[line_one_index].label,
                    menuItems[line_two_index].label,
                    menuItems[line_three_index].label
                    );
                delay(500);
                imu_read_once();
                line_cnt = 1;
                display_select = push_line_one;
                state = wait_flat;
                
            }

            else if (IMU_roll > 30.0f && line_cnt == 1) {
                push_line_two = line_one_set;
                Serial.println("line two pushed");
                oled_show_three_lines(
                    menuItems[line_one_index].label,
                    menuItems[line_two_index].label,
                    menuItems[line_three_index].label
                    );
                delay(500);
                imu_read_once();
                line_cnt = 2;
                display_select = push_line_two;
                state = wait_flat;

                
                
            }
            
            else if (IMU_roll > 30.0f && line_cnt == 2) {
                push_line_three = line_one_set;
                Serial.println("line three pushed");
                oled_show_three_lines(
                    menuItems[line_one_index].label,
                    menuItems[line_two_index].label,
                    menuItems[line_three_index].label
                    );
                imu_read_once();
                display_select = push_line_three;
                state = wait_flat;
            
                
            }

            else if (IMU_pitch < 0.0f && line_one_set > 5) {
                line_one_set = line_one_set - 1;
                delay(750);
            }

            else if (IMU_pitch > 25.0f && line_one_set < 14) {
                line_one_set = line_one_set + 1;
                delay(750);
            }

            else if (IMU_pitch < 0.0f && line_one_set == 5) {
                line_one_set = 13;
                delay(750);
            }

            else if (IMU_pitch > 20.0f && line_one_set == 14) {
                line_one_set = 6;
                delay(750);
            }
                 
           

            break;

        default:
            Serial.println("STATE: reset to INIT_fsm");
            state = INIT_fsm;
            flat_cnt = 0;
            break;
    }
}
