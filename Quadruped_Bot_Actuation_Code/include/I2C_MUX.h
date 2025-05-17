// I2C_MUX.h
// ---------
// Function declarations for controlling the PCA9548A I2C multiplexer.
// Includes functions to select a single channel or disable all channels on the multiplexer.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.



#ifndef I2C_MUX_H
#define I2C_MUX_H

#include <Arduino.h>

// Function to select a single channel on the PCA9548A I2C multiplexer
void I2C_SelectChannel(uint8_t muxAddress, uint8_t channel);

// Function to disable all channels on the PCA9548A I2C multiplexer
void I2C_DisableAllChannels(uint8_t muxAddress);

#endif