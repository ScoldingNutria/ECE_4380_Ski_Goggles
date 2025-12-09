# Ski Goggles ESP32 System

## System Overview
### Architecture Summary
#### This embedded system uses an ESP32 microcontroller to collect IMU, GPS, and temperature data at regular intervals. The data is processed and displayed on an OLED screen using an optical collimator and combiner to create a heads-up display inside ski goggles. The system operates continuously while remaining fully wearable and self-contained.

### Real-Time Operation
#### The ESP32 runs a custom scheduler-based RTOS model. All tasks are implemented as finite-state machines (FSMs) and are executed at fixed intervals for deterministic timing. Sensor readings, display updates, and networking operations run concurrently without blocking the system.

---

## Hardware Components
### Required Hardware
#### • ESP32 microcontroller  
#### • BNO055 IMU (orientation + acceleration)  
#### • NEO-6MV2 GPS module  
#### • DS18B20 temperature sensor  
#### • Hosyond OLED display  
#### • Optical collimator and combiner for heads-up projection  
#### • Battery for portable power  
#### • 3D-printed goggles frame for mounting all components  

### Goggles Frame
#### The prototype frame was created from a Thingiverse model by *ericsko* and printed using Fast Resin on a Form Labs 4 printer. The OLED holder was integrated into the frame via Fusion 360 to allow proper alignment with the combiner and collimator. The open-frame design allows easy debugging and cable routing.

---

## Sampling Processes
### Sensor Update Intervals
#### IMU, temperature, and GPS modules are read at consistent intervals to provide continuous, up-to-date system data. The ESP32 processes incoming data immediately and updates the display accordingly.

---

## Tilt Control System
### User Interaction
#### The goggles have no physical buttons; all interaction is performed using IMU-based tilt gestures.

#### • Enter Menu → sustained roll >30° for 40 IMU cycles  
#### • Scroll Menu → pitch up/down with wrap-around logic  
#### • Select Item → roll >30° while in a menu state  
#### • Exit Menu → roll <−30° returns directly to the default data view  

#### Debounce logic (“wait flat” states) prevents accidental rapid selections.

---

## RTOS Design
### Custom Scheduler
#### Because external RTOS libraries were not permitted, the system uses a custom cooperative scheduler. Each subsystem is implemented as an FSM with internal state tracking. A millisecond counter triggers each task based on a predefined execution period.

### Task Overview
#### • Sensor FSM → reads IMU → temperature → GPS → updates OLED → repeats every 1 ms  
#### • Network FSM → uploads telemetry to cloud API every 10 seconds  

#### This structure maintains strict timing guarantees and isolates subsystem behavior.

---

## Task Breakdown and Scheduling
### Sensor FSM
#### All I2C and UART sensors operate within the same FSM to ensure consistent communication timing. The display is updated once per cycle after all sensor values are refreshed.

### Network FSM
#### Telemetry (location, trail name, speed, etc.) is sent periodically to the cloud-based API used by the Squad Ski system. This FSM operates at a slower interval because network data does not require rapid updates.

---

## Goggles Frame Design
### Prototyping Details
#### The prototype used for development was printed in resin for speed and detail accuracy. Components were secured with adhesive due to resin brittleness. While not a final housing, this design enables easy diagnostics and testing of display alignment and sensor placement.

---

## Device Limitations
### Known Limitations
#### • The OLED screen can be difficult to view in bright snow conditions.  
#### • The prototype is not waterproof and requires weatherproofing for real-world use.  
#### • Resin construction is brittle; a permanent frame should be printed in PLA/ABS or manufactured in a durable material.  
#### • Exposed wiring must be enclosed and protected before deployment on the slopes.

---

## Build and Flash Instructions
### Requirements
#### • PlatformIO  
#### • ESP32 board  
#### • GPS, IMU, OLED, and temperature sensor properly wired  

### Build Steps
#### 1. Clone the repository  
#### `git clone <repo-url>`
#### 2. Open the project in PlatformIO / VS Code  
#### 3. Build and flash  
#### `pio run --target upload`
#### 4. Open serial monitor  
#### `pio device monitor`

---

## API Information
### Squad Ski API
#### Telemetry is uploaded to the Squad Ski API for tracking active riders:
#### https://github.com/Nikowarren7/squad_ski_api

### Data Uploaded
#### • User ID  
#### • Latitude & longitude  
#### • Current trail name  
#### • Active status  
#### • Additional rider metadata as needed

---
