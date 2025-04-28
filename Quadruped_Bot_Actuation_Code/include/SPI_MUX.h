// SPI_MUX.h
// This file contains the function declarations for controlling the SPI multiplexer
// It includes functions to select a specific channel and activate/deactivate the multiplexer


#ifndef SPI_MUX_H
#define SPI_MUX_H

#include <Arduino.h>

// Function declarations
void SPI_CS_MUX(uint8_t newChannel);
void ACTIVATE_MUX(uint8_t SPI_channel);
void DEACTIVATE_MUX();

#endif