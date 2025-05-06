// SPI_NCDR_FCTNS.h
// This file contains the function declarations for controlling the SPI NCDR encoder
// It includes functions to read the encoder position, reset the encoder, and read turns   




#include "PinAssignments.h"
#include "SPI_NCDR_FCT.h"
#include "SPI_MUX.h" // Include multiplexer functions

static uint16_t offsets[16] = {0}; // Array to store offsets for up to 16 channels

float readEncoderPosition(uint8_t channel) {
  uint16_t position = 0;

  // Activate the multiplexer for the desired channel
  ACTIVATE_MUX(channel);
  delayMicroseconds(3); // Small delay to establish CS for the encoder

  // Start SPI transaction with SPI Mode 0 and 100kHz clock speed
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

  // Send dummy bytes and read the 16-bit response
  position = SPI.transfer(0x00) << 8; // Send dummy data and receive 8 bits
  delayMicroseconds(3); // Small delay as prescribed by the datasheet
  position |= SPI.transfer(0x00);
  delayMicroseconds(3); // Small delay as prescribed by the datasheet

  // End SPI transaction
  SPI.endTransaction();
  delayMicroseconds(3); // Small delay as prescribed by the datasheet

  // Deactivate the multiplexer
  DEACTIVATE_MUX();

  // Extract the 12-bit position (ignore the first 2 bits and last 2 bits)
  position = (position >> 2) & 0x0FFF; // Mask to get the 12-bit value

  // Apply the offset
  if (position >= offsets[channel]) {
      position -= offsets[channel];
  } else {
      position = (4096 + position) - offsets[channel]; // Handle wrap-around
  }
  float degrees = (position * 360.0) / 4096.0; // Convert to degrees
  return degrees;
}
  
int16_t readTurns(uint8_t channel) {
  // Activate the multiplexer for the desired channel
  ACTIVATE_MUX(channel);
  delayMicroseconds(3); // Small delay to establish CS for the encoder

  // Start SPI transaction
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

  // Send the command sequence to read turns
  SPI.transfer(0x00); // First byte
  delayMicroseconds(3);
  SPI.transfer(0xA0); // Second byte
  delayMicroseconds(40); // Small delay as prescribed by the datasheet
  uint16_t highByte = SPI.transfer(0x00); // Third byte (receive high byte of turns)
  delayMicroseconds(3);
  uint16_t lowByte = SPI.transfer(0x00); // Fourth byte (receive low byte of turns)
  delayMicroseconds(3);
  // End SPI transaction
  SPI.endTransaction();
  DEACTIVATE_MUX();
  
  uint16_t rawTurns = (highByte << 8) | lowByte;
  
 // Right-shift the combined value by 2 bits and mask to extract the middle 12 bits
 int16_t turns = (rawTurns >> 2) & 0x0FFF;

  return turns;
}




//Command to reset the encoder. This does not set the zero point so an offset is needed.


void resetEncoder(uint8_t channel) {

  

    // Activate the multiplexer for the desired channel
    ACTIVATE_MUX(channel);
    delayMicroseconds(3); // Small delay to establish CS for the encoder

    // Start SPI transaction
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

    // Send the first byte (always 0x00)
    SPI.transfer(0x00);
    delayMicroseconds(3); // Small delay as prescribed by the datasheet

    // Send the second byte (reboot command 0x60)
    SPI.transfer(0x60);
    delayMicroseconds(3); // Small delay as prescribed by the datasheet

    // End SPI transaction
    SPI.endTransaction();

    // Deactivate the multiplexer
    DEACTIVATE_MUX();
    // Read the raw encoder position directly after reset
    offsets[channel] = 0; // Reset the offset to 0 for this channel
    delay(200); // Wait for the encoder to reboot and stabilize


    ACTIVATE_MUX(channel);
    delayMicroseconds(3);

    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

    // Send dummy bytes to read the raw position
    uint16_t rawPosition = SPI.transfer(0x00) << 8; // Send dummy data and receive 8 bits
    delayMicroseconds(3);
    rawPosition |= SPI.transfer(0x00);
    delayMicroseconds(3);

    SPI.endTransaction();
    DEACTIVATE_MUX();

    // Extract the 12-bit position (ignore the first 2 bits and last 2 bits)
    rawPosition = (rawPosition >> 2) & 0x0FFF;



// Store the raw position as the offset for this channel
offsets[channel] = rawPosition;



Serial.print("Encoder on channel ");
Serial.print(channel);
Serial.print(" has been rebooted. Offset set to: ");
Serial.println(offsets[channel]);
}