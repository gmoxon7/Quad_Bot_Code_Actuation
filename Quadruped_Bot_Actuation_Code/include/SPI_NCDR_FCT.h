// SPI_NCDR_FCTNS.h
// This file contains the function declarations for controlling the SPI NCDR encoder
// It includes functions to read the encoder position, reset the encoder, and read turns
//it also requires the use of the SPI_MUX.h file to select the correct channel on the SPI multiplexer



#ifndef SPI_NCDR_FCT_H
#define SPI_NCDR_FCT_H

#include <Arduino.h>
#include <SPI.h>

// Function declarations
float readEncoderPosition(uint8_t channel);
int16_t readTurns(uint8_t channel);
void resetEncoder(uint8_t channel);

#endif