/*
 * MotorDriver_LP3943.h
 * 
 * LP3943 I2C Motor Driver Interface Header
 * 
 * Description:
 *   Provides function prototypes for controlling the LP3943 LED driver
 *   as a PWM controller for the VNH7070 full bridge motor controller.
 *   Includes initialization, motion control, register access, and ADC/limit functions.
 * 
 * Hardware:
 *   - LP3943 LED Driver (I2C)
 *   - VNH7070 Full Bridge Motor Controller
 *   - Compatible with Arduino/Teensy platforms
 * 
 * Author: Greg Moxon
 * Organisation: Moxon Electronics
 * Date: 2025-06-08
 */


#ifndef MOTORDRIVER_LP3943_H
#define MOTORDRIVER_LP3943_H

#include "Arduino.h"
#include "Wire.h"
#include "I2C_FCT.h" // Include the I2C functions header file
#include "I2C_MUX.h" // Include the I2C multiplexer functions header file

void motorDriverInit(uint8_t mux_channel, uint8_t address);
void motorDriverRegControl(uint8_t mux_channel, uint8_t address, bool enable);
void variableMotionControl(uint8_t mux_channel, uint8_t address, uint8_t speed, bool direction);
void setMotionControl(uint8_t mux_channel, uint8_t address, uint8_t speedOne, uint8_t speedTwo);
void defaultMotionControl(uint8_t mux_channel, uint8_t address, uint8_t defaultSpeed, bool direction);
void motorDriverStop(uint8_t mux_channel, uint8_t address);


uint8_t readLimitTriggers(uint8_t mux_channel, uint8_t address);
float readCurrentEstimate(uint8_t mux_channel, uint8_t address);


#endif // MOTORDRIVER_LP3943_H