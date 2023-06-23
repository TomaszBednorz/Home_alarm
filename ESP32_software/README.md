# Home_alarm

Project for the class "Sensors in Embedded Applications"

## 1. Hardware
- ESP32-DevKitC (ESP-WROOM-32)
- HY-SRF05 ultrasonic sensor
- Photocell to gate (IR based)

## 2. Functionalities
- The MCU gather data: distance from the ltrasonic sensor and logic output from the photocell.
- The raw data is send via CAN protocol. Each data type has its own own frame ID.

## 3. TODO list
### Software
- [x] HY-SRF05 driver
- [x] IR driver
- [x] CAN driver (abstraction layer)
- [x] Application
### Others
- [x] Comments in the software
- [x] Documentation
