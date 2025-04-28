// SPI_NCDR_FCTNS.h
// This file contains the function declarations for controlling the SPI NCDR encoder
// It includes functions to read the encoder position, reset the encoder, and read turns   




#include "PinAssignments.h"
#include "SPI_NCDR_FCTNS.h"
#include "SPI_MUX.h" // Include multiplexer functions



uint16_t readEncoderPosition(uint8_t channel) {
    uint16_t position = 0;
  
    // Activate the multiplexer for the desired channel
    ACTIVATE_MUX(channel);// puts strobe in the correct state latches the outputs to the last input logic
    delayMicroseconds(3); // Small delay to establish cs for amt223cv 
    // Start SPI transaction with SPI Mode 0 and 100kHz clock speed
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  
    // Send dummy bytes and read the 16-bit response
      position = SPI.transfer(0x00) <<8 ; // Send dummy data and receive 8 bits?
      delayMicroseconds(3); // Small delay as prescribed by the amt223cv datasheet
      position |= SPI.transfer(0x00);
      delayMicroseconds(3); // Small delay as prescribed by the amt223cv datasheet
  
    // End SPI transaction
    SPI.endTransaction();
  delayMicroseconds(3); // Small delay as prescribed by the amt223cv datasheet
    // Deactivate the multiplexer
    DEACTIVATE_MUX();
  
    // Extract the 12-bit position (ignore the first 2 bits and last 2 bits)
    position = (position >> 2) & 0x0FFF; // Mask to get the 12-bit value
    // The position is now in the range of 0 to 4095 (12 bits)
  
    return position;
  }
  