// I2C_FCT.h
// ---------
// Function declarations and includes for I2C communication.
// Provides functions to read and write data to I2C devices using the Wire library.
// Designed for use with Arduino and compatible boards.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#ifndef I2C_FCT_H
#define I2C_FCT_H

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

//WIRE1 functions
// Function to write to a specific I2C address on the second I2C channel

void I2C_WR1(unsigned char ADDRESS, unsigned char I2C_OPERATION);
// Overloaded function for devices with chip address, register address, and data byte
void I2C_WR1(unsigned char chipAddress, unsigned char registerAddress, unsigned char dataByte);

//Function to read from specific addresses
// Overloaded function for devices with only chip address
unsigned char I2C_RD1(unsigned char ADDRESS);
// Overloaded function for devices with chip address and register address
unsigned char I2C_RD1(unsigned char chipAddress, unsigned char registerAddress);


#endif