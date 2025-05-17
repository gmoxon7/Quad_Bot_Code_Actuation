// PinAssignments.h
// ----------------
// Pin assignments for the Teensy 4.0 microcontroller on the central control board project.
// Defines pin numbers for precharge/discharge enable, charge enable, multiplexer control, 
// and other board functions using macros for easy reference.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#ifndef PIN_ASSIGNMENTS_H
#define PIN_ASSIGNMENTS_H

// Pin definitions
#define PRE_DSG_EN 2 //precharge discharge enable
#define DSG_EN 3  //discharge enable
#define CHG_EN 4  //charge enable

#define STROBE 5 //strobe signal, This latches multiplexer when low, live update when high
#define CS_1 6 //Chip select pin numbers to the multiplexer. CS1 is LSB, CS4 is MSB
#define CS_2 7
#define CS_3 8
#define CS_4 9
#define INHIBIT 10 //INHIBIT is high all outputs on SPI multiplexer are 0 

#define NFAULT 14 //Signal to the L9961 bms chip recieves information  status of the battery pack, input
#define RDY 15 //RDY interrupt output. will be an input

#define EN_PIN 20 //output that turns on a range of chips on the control board, output
#define NRESET 21 //reset pin to the I2C multiplexer, output
#define SERVO_REG_ENABLE 22 //enable pin for the servo regulator, output

#define PACK_SNS 23 //pack sense input, input

#endif