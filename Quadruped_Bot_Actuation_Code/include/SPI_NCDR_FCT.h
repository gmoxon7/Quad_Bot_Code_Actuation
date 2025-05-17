// SPI_NCDR_FCT.h
// --------------
// Function declarations for controlling the SPI NCDR encoder.
// Includes functions to read the encoder position, reset the encoder, and read turns.
// Requires the use of SPI_MUX.h to select the correct channel on the SPI multiplexer.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#ifndef SPI_NCDR_FCT_H
#define SPI_NCDR_FCT_H

#include <Arduino.h>
#include <SPI.h>

// Function declarations
float readEncoderPosition(uint8_t channel);
int16_t readTurns(uint8_t channel);
void resetEncoder(uint8_t channel);

#endif