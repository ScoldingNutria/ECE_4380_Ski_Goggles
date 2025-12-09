# Ski_Goggles_ESP32_System

## This project implements an ESP32-based telemetry and display platform designed for ski goggles.
## The system integrates GPS, an inertial measurement unit, temperature sensing, and an OLED heads-up display.
## A cloud API (Application Programming Interface) is used for real-time telemetry uploads and rider tracking.

## The system performs live trail identification, heading calculation, speed monitoring, temperature reading,
## and g-force tracking while maintaining a responsive display and motion-controlled user interface.

## The firmware uses a lightweight RTOS-style scheduler to coordinate subsystem updates.
## Each module operates in periodic time slices to maintain smooth, non-blocking performance.

## The system continuously samples IMU data for heading, pitch, roll, and g-force.
## GPS data is used to determine speed, altitude, satellite count, and geographic position.
## Temperature data is acquired using a OneWire sensor.
## The OLED displays three dynamic lines of information updated in real time.
## Trail identification is performed using nearest-point matching against predefined trail coordinates.
## Telemetry is periodically uploaded to the Squad Ski API.
## Menu navigation is performed using head-tilt gestures detected by the IMU.

## The file main.cpp initializes Wi-Fi (Wireless Fidelity), configures the scheduler, and manages the main loop.
## It runs both the Ski FSM and the Network FSM at their assigned update rates.

## The file ski_fsm.cpp contains the primary finite-state machine for system behavior.
## It handles IMU, GPS, and temperature sampling, trail detection, OLED updates, menu logic, and safety states.

## The file network_fsm.cpp manages API communication in a non-blocking loop.
## It supports telemetry uploads and future expansion for user registration and rider queries.

## The file api_client.cpp constructs and sends JSON telemetry packets.
## It trims trail names, builds URLs, performs HTTP POST requests, and reports API success or failure.

## The file IMU_sense.cpp manages the BNO055 IMU.
## It provides heading, pitch, roll, linear acceleration, and g-force calculations with peak tracking.

## The file GPS_sense.cpp uses TinyGPS++ to provide latitude, longitude, altitude, speed, and satellite count.
## It also integrates with the trail-identification system to determine the nearest ski trail.

## The file TEMP_sense.cpp reads from a OneWire temperature sensor.
## It reports values in both Celsius and Fahrenheit.

## The file OLED_write.cpp renders the monochrome OLED display.
## It includes auto-mirroring, large and small text modes, custom three-line displays, and pixel-level glyph drawing.

## The files skills.cpp and skills.h provide shared global variables, menu definitions, direction mapping,
## dynamic labels, and the URL builder function used throughout the project.

## The files trail_map.cpp and trail_map.h define trail coordinates and implement the nearest-point trail algorithm.

## Menu interaction is hands-free and uses head movements detected by the IMU.
## Rolling right selects items, pitching up or down scrolls, and neutral roll confirms actions.
## The OLED displays three lines at a time with live sensor values and dynamic labels.

## Telemetry is uploaded to https://squad-ski-api.onrender.com.
## Data fields include user_id, active status, latitude, longitude, trail name, and a server timestamp.

## Building this project requires PlatformIO, an ESP32 development board, and sensors including the BNO055,
## GPS module, OneWire temperature sensor, and SSD1306 OLED.

## To build and upload the firmware:
## Clone the repository using “git clone <repo-url>”.
## Open the project in PlatformIO or Visual Studio Code.
## Build and upload using “pio run --target upload”.
## View serial output using “pio device monitor”.
