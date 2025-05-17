// SPI_MUX.h
// ---------
// Function declarations for controlling the SPI multiplexer on the central control board.
// Includes functions to select a specific channel and activate/deactivate the multiplexer.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#ifndef SPI_MUX_H
#define SPI_MUX_H

#include <Arduino.h>

// Function declarations
void SPI_CS_MUX(uint8_t newChannel);
void ACTIVATE_MUX(uint8_t SPI_channel);
void DEACTIVATE_MUX();

#endif