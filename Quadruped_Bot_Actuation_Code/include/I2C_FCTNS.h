// Header file for the I2C functions

// This file contains the function declarations and includes necessary libraries
// for I2C communication. It is used to read and write data to I2C devices.
// The functions are designed to work with the Wire library, which is a standard
// library for I2C communication in Arduino and compatible boards.
// The functions include I2C_WR for writing data to a specific I2C address and
// I2C_RD for reading data from a specific I2C address.

#ifndef I2C_FCTNS_H
#define I2C_FCTNS_H

#include <Arduino.h>
#include <Wire.h> // Include the Wire library for I2C communication

// Function declarations


// Function to write to a specific I2C address
// Overloaded function for devices with only chip address and data byte
void I2C_WR(unsigned char ADDRESS, unsigned char I2C_OPERATION);
// Overloaded function for devices with chip address, register address, and data byte
void I2C_WR(unsigned char chipAddress, unsigned char registerAddress, unsigned char dataByte);

//Function to read from specific addresses
// Overloaded function for devices with only chip address
unsigned char I2C_RD(unsigned char ADDRESS);
// Overloaded function for devices with chip address and register address
unsigned char I2C_RD(unsigned char chipAddress, unsigned char registerAddress);



#endif