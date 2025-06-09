/*
 * MotorDriver_LP3943.cpp
 * 
 * LP3943 I2C Motor Driver Interface Source
 * 
 * Description:
 *   Implements functions for controlling the LP3943 LED driver
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


 #include "MotorDriver_LP3943.h"
 
 #include "Wire.h"
 #include "I2C_FCT.h" // Include the I2C functions header file
 #include "I2C_MUX.h" // Include the I2C multiplexer functions header file


 

 void motorDriverInit(uint8_t mux_channel,uint8_t i2c_addr) {

    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer
    
    // Example register addresses and default values (replace with your actual values)
    const uint8_t registers[] = {0x02, 0x03, 0x04, 0x07}; // register addresses
    const uint8_t values[]    = {0x00, 0x80, 0x00, 0x55}; // Sets the Prescalers to maximum frequency((1+DATA)/160=1/6.35ms the pwm0 needs determining for speed at 50%. 0X07 is the driver, wants to be all inactive.

    for (uint8_t i = 0; i < sizeof(registers); i++) {
        I2C_WR(i2c_addr, registers[i], values[i]);
        delay(2); // Small delay for I2C stability
    }
}


void motorDriverRegControl(uint8_t mux_channel,uint8_t i2c_addr, bool enable) {

    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer
    
    // Enable or disable the motor driver by writing to the corresponding register
    if (enable) {
        I2C_WR(i2c_addr, 0X08, 0X15); // 0x15 is the value to enable the motor driver and the adc
        I2C_WR(i2c_addr, 0X08, 0X05); // 0x15 is the value to enable the motor driver and the adc
    } else {
        I2C_WR(i2c_addr, 0X08, 0X00);
    }
}

void variableMotionControl(uint8_t mux_channel, uint8_t i2c_addr, uint8_t speed, bool direction) {

    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer
    
    // Set the speed and direction for the motor
    I2C_WR(i2c_addr, 0x05, 255- speed); // Write speed to register 0x05, this sets PWM1 in the chip to a certain duty cycle.
    
    if (direction) {
        I2C_WR(i2c_addr, 0x07, 0xC4); // Set direction to forward (1)
    } else {
        I2C_WR(i2c_addr, 0x07, 0xD1); // Set direction to reverse (0)
    }

}

void setMotionControl(uint8_t mux_channel, uint8_t i2c_addr, uint8_t speedOne, uint8_t speedTwo) {

    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer
    
    // Set the speed for both motors
    I2C_WR(i2c_addr, 0x03, 255-speedOne); // Write speed to register 0x05 for motor one
    I2C_WR(i2c_addr, 0x05, 255-speedTwo); // Write speed to register 0x06 for motor two
}


void defaultMotionControl(uint8_t mux_channel, uint8_t i2c_addr, uint8_t speedLevel, bool direction) {
    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer

    // Set speed bits (MSBs)
    uint8_t speedBits = 0;
    if (speedLevel == 3) {
        speedBits = 0b00000000;
    } else if (speedLevel == 2) {
        speedBits = 0b11000000;
    } else if (speedLevel == 1) {
        speedBits = 0b10000000;
    } else {
        speedBits = 0b01000000; // Or set a default/fault value if needed
    }

    // Set direction code (6 LSBs)
    uint8_t directionCode;
    if (direction) {
        directionCode = 0x04; // Example: forward code (replace with your value)
    } else {
        directionCode = 0x11; // Example: reverse code (replace with your value)
    }

    uint8_t regValue = speedBits | (directionCode & 0x3F);

    // Write the combined value to the register (e.g., 0x07)
    I2C_WR(i2c_addr, 0x07, regValue);
}


void motorDriverStop(uint8_t mux_channel, uint8_t i2c_addr) {

    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer
    
    // Stop the motor by setting speed to 0
    I2C_WR(i2c_addr, 0x07, 0x55); // Write 0 to register 0x05 to stop the motor
    
}



uint8_t readLimitTriggers(uint8_t mux_channel, uint8_t i2c_addr) {

    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel); // Select the appropriate channel on the I2C multiplexer
    
    // Read the limit triggers from the motor driver
    uint8_t limitTriggers = (0b00001111 & (I2C_RD(i2c_addr, 0x00))); // Read from register 0x0A (example register for limit triggers)
    
    return limitTriggers; // Return the read value
}


float readCurrentEstimate(uint8_t mux_channel, uint8_t i2c_addr) {
    I2C_SelectChannel(I2C_MUX_ADDRESS, mux_channel);

    // Read the current estimate from the motor driver (upper 4 bits of register 0x01)
    uint8_t raw = (I2C_RD(i2c_addr, 0x01) & 0b11111000) >> 3;

    // Scale to max current (0–15 maps to 0–6.1467A)
    float current = (raw / 31.0f) * 6.1467f;

    return current;
}