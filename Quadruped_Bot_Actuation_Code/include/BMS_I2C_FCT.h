// BMS_I2C_FCT.h: This header file contains function declarations for I2C communication with a BMS (Battery Management System) module.
// tHIS bms IS THE l9961 from ST Microelectronics
// It includes functions to read and write data to the BMS module using I2C communication.

#ifndef BMS_I2C_FCT_H
#define BMS_I2C_FCT_H

#include <Arduino.h>
#include <Wire.h> // Include the Wire library for I2C communication

uint16_t readBMSData(uint8_t chipAddress, uint8_t registerAddress);


#endif // BMS_I2C_FCT_H