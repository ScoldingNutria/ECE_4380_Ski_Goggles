#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "skills.h"

#define TEMP_PIN 4

static OneWire oneWire(TEMP_PIN);
static DallasTemperature sensors(&oneWire);
static bool temp_online = false;

void temp_init() {
    sensors.begin();
    sensors.setResolution(9);
    temp_online = true;
    Serial.println("TEMP SENSOR ONLINE");
}

void temp_read_once() {
    if (!temp_online) return;

    sensors.requestTemperatures();
    TEMP_celsius = sensors.getTempCByIndex(0);
    TEMP_F = (((TEMP_celsius* 9) / 5) + 32);

    Serial.printf("TEMP: %.1fC\n", TEMP_celsius);
}
