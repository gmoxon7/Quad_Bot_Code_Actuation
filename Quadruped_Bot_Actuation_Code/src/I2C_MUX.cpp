// I2C_MUX.cpp
// -----------
// Implementation of functions for controlling the PCA9548A I2C multiplexer.
// Includes functions to select a specific channel and disable all channels on the multiplexer.
// Only one channel is specified to be active at a time, but it is possible to use all at once.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.



#include "PinAssignments.h"
#include "I2C_MUX.h"
#include "I2C_FCT.h" // Include the I2C_FCTNS header file

// Global variable to store the currently active channel
static int8_t currentChannel = -1; // -1 indicates no channel is currently selected



// Function to select a single channel on the PCA9548A I2C multiplexer
void I2C_SelectChannel(uint8_t muxAddress, uint8_t channel) {
  // Validate the channel number (must be between 0 and 7)
  if (channel > 7) {
    Serial.println("\nError: Invalid channel requested! Must be between 0 and 7.");
    return;
  }

  // Check if the requested channel is already active
  if (currentChannel == channel) {
    Serial.print("\nI2C Multiplexer: Channel ");
    Serial.print(channel);
    Serial.println(" is already active. No action taken.");
    return;
  }

  // Create the control byte with only the desired channel active
  uint8_t controlByte = (1 << channel);

  // Use the I2C_WR function from I2C_FCTNS to send the control byte
  I2C_WR(muxAddress, controlByte);

  // Update the currently active channel
  currentChannel = channel;

  Serial.print("\nI2C Multiplexer: Channel ");
  Serial.print(channel);
  Serial.println(" selected.");
}

// Function to disable all channels on the PCA9548A I2C multiplexer
void I2C_DisableAllChannels(uint8_t muxAddress) {
  // Use the I2C_WR function from I2C_FCTNS to send a control byte of 0x00
  I2C_WR(muxAddress, 0x00);

  // Update the currently active channel to indicate no channel is active
  currentChannel = -1;

  Serial.println("\nI2C Multiplexer: All channels disabled.");
}