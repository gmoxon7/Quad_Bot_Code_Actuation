// I2C_FCT.cpp
// -----------
// Implementation of I2C functions for reading and writing data to I2C devices.
// Provides overloaded functions for single and dual I2C channels, supporting register and data byte access.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#include "PinAssignments.h"
#include "I2C_FCT.h"

// Function to write to a specific I2C address on the defualt i2c channel.
// Overloaded function for devices with only chip address and data byte
void I2C_WR(uint8_t chipAddress, uint8_t dataByte) {
  Wire.beginTransmission(chipAddress);
  Wire.write(dataByte);
  Wire.endTransmission();
}

// Overloaded function for devices with chip address, register address, and data byte
void I2C_WR(uint8_t chipAddress, uint8_t registerAddress, uint8_t dataByte) {
  Wire.beginTransmission(chipAddress);
  Wire.write(registerAddress); // Write the register address first
  Wire.write(dataByte);        // Then write the data byte
  Wire.endTransmission();
}




// Function to read from an I2C device with only one register
// This only takes one address and returns one byte of data
// Overloaded function for devices with only chip address
uint8_t I2C_RD(uint8_t chipAddress) {
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
uint8_t I2C_RD(uint8_t chipAddress, uint8_t registerAddress) {
  Wire.beginTransmission(chipAddress); // Start communication with the device
  Wire.write(registerAddress);         // Specify the register to read from
  Wire.endTransmission(false);         // Send a repeated start condition
  Wire.requestFrom(chipAddress, 1);    // Request 1 byte from the device

  while (!Wire.available()) {
    // Wait until data is available
  }

  return Wire.read(); // Read and return the received byte
}



// wire1 functions

// Function to write to a specific I2C address on the second i2c channel.
// Overloaded function for devices with only chip address and data byte
void I2C_WR1(uint8_t chipAddress, uint8_t dataByte) {
  Wire1.beginTransmission(chipAddress);
  Wire1.write(dataByte);
  Wire1.endTransmission();
}

// Overloaded function for devices with chip address, register address, and data byte
void I2C_WR1(uint8_t  chipAddress, uint8_t registerAddress, uint8_t dataByte) {
  Wire1.beginTransmission(chipAddress);
  Wire1.write(registerAddress); // Write the register address first
  Wire1.write(dataByte);        // Then write the data byte
  Wire1.endTransmission();
}




// Function to read from an I2C device with only one register
// This only takes one address and returns one byte of data
// Overloaded function for devices with only chip address
uint8_t I2C_RD1(uint8_t chipAddress) {
  Wire1.beginTransmission(chipAddress); // Start communication with the device
  Wire1.endTransmission(false);         // Send a repeated start condition
  Wire1.requestFrom(chipAddress, 1);    // Request 1 byte from the device

  while (!Wire1.available()) {
    // Wait until data is available
  }

  return Wire1.read(); // Read and return the received byte
}

// Overloaded function for devices with chip address and register address
// This function reads a byte from a specific register of an I2C device
uint8_t I2C_RD1(uint8_t chipAddress, uint8_t registerAddress) {
  Wire1.beginTransmission(chipAddress); // Start communication with the device
  Wire1.write(registerAddress);         // Specify the register to read from
  Wire1.endTransmission(false);         // Send a repeated start condition
  Wire1.requestFrom(chipAddress, 1);    // Request 1 byte from the device

  while (!Wire1.available()) {
    // Wait until data is available
  }

  return Wire1.read(); // Read and return the received byte
}