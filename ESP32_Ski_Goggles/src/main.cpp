#include <Arduino.h>
#include <WiFi.h>           
#include "skills.h"
#include "ski_fsm.h"
#include "network_fsm.h"    


typedef struct {
    int state;
    unsigned long period;      
    unsigned long elapsedTime;
    int (*Function)(int);
} task;


const unsigned long GCD_PERIOD = 5;           
const unsigned int NUM_TASKS = 2;


const unsigned long PERIOD_SKI_FSM     = 1;    
const unsigned long PERIOD_NETWORK_FSM = 200; 

task tasks[NUM_TASKS];


int TickFct_SkiFSM(int state) {
    ski_fsm();
    return state;  
}

int TickFct_NetworkFSM(int state) {
    network_fsm();
    return state;  
}


void TimerISR() {
    for (unsigned int i = 0; i < NUM_TASKS; i++) {
        tasks[i].elapsedTime += GCD_PERIOD;
        if (tasks[i].elapsedTime >= tasks[i].period * GCD_PERIOD) {
            tasks[i].state = tasks[i].Function(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
    }
}


void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    Serial.println("\n BOOT ");

   
    Serial.print("Connecting to Wi-Fi");
    WiFi.begin("NikoiPhone", "nikoswifi");  // ← CHANGE ME

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi CONNECTED");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    
    tasks[0].state = 0;
    tasks[0].period = PERIOD_SKI_FSM;           
    tasks[0].elapsedTime = 0;
    tasks[0].Function = &TickFct_SkiFSM;

    
    tasks[1].state = 0;
    tasks[1].period = PERIOD_NETWORK_FSM;       
    tasks[1].elapsedTime = 0;
    tasks[1].Function = &TickFct_NetworkFSM;

    Serial.println("Tasks initialized");
}


void loop() {
    static unsigned long lastTick = 0;
    unsigned long now = millis();

    if (now - lastTick >= GCD_PERIOD) {
        lastTick = now;
        TimerISR();
    }
}