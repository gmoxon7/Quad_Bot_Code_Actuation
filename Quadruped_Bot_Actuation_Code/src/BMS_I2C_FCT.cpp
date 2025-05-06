//BMS_I2C_FCT.cpp
// This file contains the implementation of I2C functions for reading and writing data to a BMS module

#include "PinAssignments.h"
#include "BMS_I2C_FCT.h" // Include the header file for BMS I2C functions


// Single Read Function for L9961
uint16_t readBMSData(uint8_t chipAddress, uint8_t registerAddress) {
    uint16_t data = 0;

    // Step 1: Send the slave address with the write bit and the register address
    Wire1.beginTransmission(chipAddress);
    Wire1.write(registerAddress);
    Wire1.endTransmission(true); // Send a repeated start condition
    delayMicroseconds(2); // Small delay to allow the device to process the request (tbuf)
    // Step 2: Send the slave address with the read bit
    Wire1.requestFrom(chipAddress, (uint8_t)2, true); // Request 2 bytes from the slave

    // Step 3: Read the two bytes of data
    if (Wire1.available() >= 2) {
        uint8_t highByte = Wire1.read(); // Read the first byte (MSB)
        uint8_t lowByte = Wire1.read();  // Read the second byte (LSB)
        data = (highByte << 8) | lowByte; // Combine the two bytes into a 16-bit value
    }

    return data;
}