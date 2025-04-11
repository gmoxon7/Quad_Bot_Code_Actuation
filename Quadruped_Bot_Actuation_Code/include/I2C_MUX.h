//Header file for I2C_MUX.cpp
// This file contains the function declarations for controlling the PCA9548A I2C multiplexer

#ifndef I2C_MUX_H
#define I2C_MUX_H

#include <Arduino.h>

// Function to select a single channel on the PCA9548A I2C multiplexer
void I2C_SelectChannel(uint8_t muxAddress, uint8_t channel);

// Function to disable all channels on the PCA9548A I2C multiplexer
void I2C_DisableAllChannels(uint8_t muxAddress);

#endif