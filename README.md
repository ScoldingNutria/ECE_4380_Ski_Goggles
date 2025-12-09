# Ski_Goggles_ESP32_System

## Overview
### System Description
#### This project implements an ESP32-based telemetry and display system designed for integration into ski goggles.
#### The system collects IMU, GPS, and temperature data and renders real-time information to an OLED heads-up display.
#### A cloud API (Application Programming Interface) is used to upload rider telemetry for remote monitoring.
#### The API for this system is hosted in a separate repository:  
#### https://github.com/Nikowarren7/squad_ski_api

### Design Goals
#### The system prioritizes reliability, non-blocking sensor workflows, smooth display updates, and intuitive motion-based controls.
#### A lightweight RTOS-style scheduler ensures periodic execution of all subsystems without interfering with one another.

---

## System Architecture
### Core Functionality
#### • Continuous IMU sampling (heading, pitch, roll, g-force)  
#### • GPS updates (speed, altitude, coordinates, satellite count)  
#### • Temperature readings via a OneWire sensor  
#### • Real-time OLED display using a custom three-line layout  
#### • Automatic trail identification using nearest-point matching  
#### • Periodic telemetry uploads to the Squad Ski API  
#### • Tilt-based menu navigation using IMU roll and pitch  

### Scheduler Structure
#### The firmware uses periodic “ticks” to coordinate subsystem updates.
#### This prevents blocking and allows every sensor and task to run predictably.

---

## File Descriptions

### main.cpp
#### Handles Wi-Fi initialization, task scheduling, and the primary timing loop.
#### Runs two key tasks:
#### • Ski FSM (high-frequency sensor loop)
#### • Network FSM (lower-frequency API loop)

### ski_fsm.cpp
#### Implements the main finite-state machine controlling system behavior.
#### Responsibilities include:
#### • Sensor sampling (IMU, GPS, temperature)  
#### • Trail detection  
#### • OLED rendering  
#### • Menu navigation using head movements  
#### • Stabilization/wait states for clean transitions  

### network_fsm.cpp
#### Manages communication with the cloud API.
#### Handles telemetry uploads and supports future extensions such as active-user queries.

### api_client.cpp
#### Formats and sends outbound JSON telemetry packets.
#### Includes:
#### • URL construction  
#### • Trail-name trimming  
#### • HTTP POST handling  
#### • Response validation  

### IMU_sense.cpp
#### Interfaces with the BNO055 IMU.
#### Provides heading, pitch, roll, linear acceleration, and g-force calculations.
#### Also maintains maximum g-force values.

### GPS_sense.cpp
#### Integrates TinyGPS++ to obtain:
#### • Latitude and longitude  
#### • Altitude  
#### • Speed (mph)  
#### • Satellite count  
#### Also supports trail identification using map coordinates.

### TEMP_sense.cpp
#### Reads temperature from a OneWire sensor and reports values in Celsius and Fahrenheit.

### OLED_write.cpp
#### Controls the SSD1306 OLED display.
#### Features include:
#### • Horizontal mirroring for goggle mounting orientation  
#### • Large and small font modes  
#### • Three-line dynamic display layout  
#### • Pixel-level custom glyphs for compact text rendering  

### skills.cpp / skills.h
#### Provides global sensor variables, menu definitions, directional heading mapping,  
#### the URL builder function, and dynamic display labels.

### trail_map.cpp / trail_map.h
#### Contains predefined GPS trail coordinates.
#### Performs nearest-point matching to determine the rider’s current trail.

---

## User Interaction
### Motion-Controlled Menu System
#### • Roll right → select menu item  
#### • Pitch up/down → scroll through options  
#### • Neutral roll → confirm or return  
#### The OLED always displays three lines, updating continuously with live sensor values.

---

## API Integration
### Endpoint
#### Telemetry is sent to:
#### https://squad-ski-api.onrender.com

### GitHub Repository for the API
#### The full backend implementation and documentation can be found here:  
#### https://github.com/Nikowarren7/squad_ski_api

### Data Fields
#### • user_id  
#### • active status  
#### • latitude and longitude  
#### • trail name  
#### • timestamp (server-assigned)  

#### This enables real-time visualization and tracking of active riders.

---

## Building & Flashing Instructions

### Requirements
#### • PlatformIO  
#### • ESP32 development board  
#### • BNO055 IMU  
#### • GPS module  
#### • OneWire temperature sensor  
#### • SSD1306 OLED display  

### Steps
#### 1. Clone the repository:  
#### `git clone <repo-url>`
#### 2. Open the project in PlatformIO or VS Code.  
#### 3. Build and upload:  
#### `pio run --target upload`
#### 4. Open the serial monitor:  
#### `pio device monitor`

---

## Future Improvements
### Planned Enhancements
#### • Expanded trail map coverage  
#### • Crash detection using g-force thresholds  
#### • Bluetooth audio alerts  
#### • Offline data logging  
#### • Enhanced OLED UI themes  
#### • Battery monitoring and power optimization  
