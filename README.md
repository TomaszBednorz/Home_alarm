# Home_alarm

## 1. Hardware
- ESP32-DevKitC (ESP-WROOM-32)
- HY-SRF05 ultrasonic sensor
- Photocell to gate (IR based)

## 2. Functionalities
- The MCU gather data (acceleration, angular rate, temperature and steps) from the IMU.
- The raw data is send via BLE protocol when MCU is connected to another device. Each data type has its own own GATT characteristic.

## 3. TODO list
### Software
- [x] HY-SRF05 driver
- [x] IR driver
- [x] CAN driver (abstraction layer)
- [x] Application
### Others
- [x] Comments in the software
- [x] Documentation
