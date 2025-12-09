#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "skills.h"

#define SDA_IMU 12
#define SCL_IMU 13




// Using address 0x29 
static Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);
static bool imu_online = false;

void imu_init() {
    Serial.println("\n IMU INIT (SDA=12, SCL=13, Addr=0x29) ");

    Wire.begin(SDA_IMU, SCL_IMU);
    delay(20);

    if (!bno.begin()) {
        Serial.println("IMU NOT FOUND");
        imu_online = false;
        return;
    }

    bno.setExtCrystalUse(true);
    imu_online = true;

    Serial.println("IMU ONLINE");
}

void imu_read_once() {
    if (!imu_online) {
        Serial.println("IMU OFFLINE");
        return;
    }

    sensors_event_t orientationData, accelData;

    
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
    bno.getEvent(&accelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

    
    IMU_heading = orientationData.orientation.x;
    IMU_roll   = orientationData.orientation.y;
    IMU_pitch    = orientationData.orientation.z;

    float g_force_raw = sqrt(
        accelData.acceleration.x * accelData.acceleration.x +
        accelData.acceleration.y * accelData.acceleration.y +
        accelData.acceleration.z * accelData.acceleration.z
    );


    IMU_gforce = g_force_raw / 9.8f + 1;
    if (IMU_gforce > IMU_gforce_max) {
        IMU_gforce_max = IMU_gforce;
    }
    
    Serial.print("Heading: "); Serial.print(IMU_heading, 1);
    Serial.print("°  Pitch: "); Serial.print(IMU_pitch, 1);
    Serial.print("°  Roll: "); Serial.print(IMU_roll, 1);
    Serial.print("°  ");

    Serial.print("G-Force: "); Serial.println(IMU_gforce, 2);
}
