// SPI_MUX.cpp
// -----------
// Implementation of functions for controlling the SPI multiplexer on the central control board.
// Includes functions to select a specific channel, activate/deactivate the multiplexer, and manage output latching.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.


#include "PinAssignments.h"
#include "SPI_MUX.h"

uint8_t MUX_channel = 0; //SPI multiplexer channel number 0 default
// put function declarations here:

//Functions



void SPI_CS_MUX(uint8_t newChannel)
 {
  //This function selects the channel on the SPI multiplexer
  //The channel is selected by setting the CS pins to the binary value of the channel number
  //The 4 CS pins are passed through a mux to get 16 outputs.
  if (newChannel > 15) {
    Serial.println("Error: Invalid channel requested! Must be between 0 and 15");
  return; // invalid channel number
  }
  MUX_channel = newChannel;
   // Set the multiplexer control pins based on the channel
   digitalWrite(CS_1, MUX_channel & 0x01);       // LSB
   digitalWrite(CS_2, (MUX_channel >> 1) & 0x01);
   digitalWrite(CS_3, (MUX_channel >> 2) & 0x01);
   digitalWrite(CS_4, (MUX_channel >> 3) & 0x01); // MSB

}

void ACTIVATE_MUX(uint8_t SPI_channel){
    // Select the desired channel
    SPI_CS_MUX(SPI_channel);
    
    // Latch the selected channel
    digitalWrite(STROBE, HIGH); //allows registers to output current input logic
    delay(10); // Small delay for stability
    digitalWrite(STROBE, LOW); // latches the outputs to last input logic, preventing change during transmission
    delay(10); // Small delay for stability
    
    // Enable the multiplexer outputs
    digitalWrite(INHIBIT, LOW); //outputs latched logic
    delayMicroseconds(3); // Small delay for stability
    
    }
    
void DEACTIVATE_MUX() {
    // Disable all multiplexer outputs
    digitalWrite(INHIBIT, HIGH);
    delayMicroseconds(3); // Small delay for stability

    }
    
    