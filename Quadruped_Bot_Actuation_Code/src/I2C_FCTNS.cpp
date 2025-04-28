// I2C_FCTNS.cpp
// This file contains the implementation of I2C functions for reading and writing data



#include "PinAssignments.h"
#include "I2C_FCTNS.h"

// Function to write to a specific I2C address
// Overloaded function for devices with only chip address and data byte
void I2C_WR(unsigned char chipAddress, unsigned char dataByte) {
  Wire.beginTransmission(chipAddress);
  Wire.write(dataByte);
  Wire.endTransmission();
}

// Overloaded function for devices with chip address, register address, and data byte
void I2C_WR(unsigned char chipAddress, unsigned char registerAddress, unsigned char dataByte) {
  Wire.beginTransmission(chipAddress);
  Wire.write(registerAddress); // Write the register address first
  Wire.write(dataByte);        // Then write the data byte
  Wire.endTransmission();
}




// Function to read from an I2C device with only one register
// This only takes one address and returns one byte of data
// Overloaded function for devices with only chip address
unsigned char I2C_RD(unsigned char chipAddress) {
  Wire.beginTransmission(chipAddress); // Start communication with the device
  Wire.endTransmission(false);         // Send a repeated start condition
  Wire.requestFrom(chipAddress, 1);    // Request 1 byte from the device

  while (!Wire.available()) {
    // Wait until data is available
  }

  return Wire.read(); // Read and return the received byte
}

// Overloaded function for devices with chip address and register address
// This function reads a byte from a specific register of an I2C device
unsigned char I2C_RD(unsigned char chipAddress, unsigned char registerAddress) {
  Wire.beginTransmission(chipAddress); // Start communication with the device
  Wire.write(registerAddress);         // Specify the register to read from
  Wire.endTransmission(false);         // Send a repeated start condition
  Wire.requestFrom(chipAddress, 1);    // Request 1 byte from the device

  while (!Wire.available()) {
    // Wait until data is available
  }

  return Wire.read(); // Read and return the received byte
}