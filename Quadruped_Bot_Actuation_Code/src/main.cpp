// main.cpp
// --------
// Main application file for the Quad_Bot_Code_Actuation project.
// This program actuates the quadruped robot, provides feedback data to the Raspberry Pi,
// and communicates over ROS (Robot Operating System).
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.



#include <Arduino.h>

//custom header file for I2C functions
#include "PinAssignments.h" // Include the pin assignments header file

#include "PWR_MNGMT_FCT.h" // Include the power management functions header file

#include "I2C_FCT.h" // Include the I2C functions header file
#include "I2C_MUX.h"    // Include the I2C multiplexer functions header file

#include "SPI_NCDR_FCT.h" // Include the SPI NCDR functions header file
#include "SPI_MUX.h" // Include the SPI multiplexer functions

#include "BMS_CoreCommands.h" // Include the BMS core commands header file
#include "BMS_SetupCommands.h" // Include the BMS setup commands header file
#include "BMS_NumericalCommands.h" // Include the BMS numerical commands header file

#include "BMS_ReadCommands.h" // Include the BMS read commands header file
#include "SetUpBMS.h" // Include the BMS setup header file

#include "MotorDriver_LP3943.h" // Include the motor driver LP3943 header file


#include <Wire.h>


#include <SPI.h> // for the rotary encoder

//Definitions
#define WIRE Wire // for teensy 4.0 this is the i2c library on pins 18(sda) and 19(scl)
#define WIRE Wire1 // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(scl1)



uint8_t MOTOR_DRIVER_DEFAULT_ADDRESS = 0x60; // Default I2C address for the LP3943 motor driver
float degrees = 0; // Variable to store the encoder position

uint8_t chipAddress = 0x49; // Replace with the actual I2C address of the L9961
uint8_t registerAddress = 0x0; // checking vb value.

float packVoltage = 0.0; // Variable to store the pack voltage


//Testing Variables
char inputBuffer[32];
uint8_t inputPos = 0;

// put setup code here, to run once:

void setup() {
  // put your setup code here, to run once:

  //defining pin directions
pinMode(PRE_DSG_EN, OUTPUT);
pinMode(DSG_EN, OUTPUT);  
pinMode(CHG_EN, OUTPUT);

pinMode(STROBE, OUTPUT);
pinMode(CS_1, OUTPUT);
pinMode(CS_2, OUTPUT);
pinMode(CS_3, OUTPUT);
pinMode(CS_4, OUTPUT);
pinMode(INHIBIT, OUTPUT);

pinMode(NFAULT, INPUT);
pinMode(RDY, INPUT);

pinMode(EN_PIN, OUTPUT);
pinMode(NRESET, OUTPUT);

pinMode(SERVO_REG_ENABLE, OUTPUT);

pinMode(PACK_SNS, INPUT);

//setting pin  positions for startup
digitalWrite(PRE_DSG_EN, LOW);
digitalWrite(DSG_EN, LOW);
digitalWrite(CHG_EN, LOW);
digitalWrite(STROBE, LOW);
digitalWrite(CS_1, LOW);
digitalWrite(CS_2, LOW);
digitalWrite(CS_3, LOW);
digitalWrite(CS_4, LOW);
digitalWrite(INHIBIT, HIGH);
digitalWrite(EN_PIN, LOW);
digitalWrite(NRESET, LOW);
digitalWrite(SERVO_REG_ENABLE, LOW);



delay(1000); //delay for the control board to initialize

  Serial.begin(9600);
  Wire.begin(); //initialize the i2c bus
  Wire1.begin(); //initialize the i2c bus
  
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Initialized");
  delay(100);
  SPI.begin(); //initialize the SPI bus
  delay(100);
  Serial.println("\nSPI Initialized");

  digitalWrite(EN_PIN, HIGH);
  delay(10); //delay for the Peripheral ICS to initialize
  digitalWrite(NRESET, LOW);
  delay(10);
  digitalWrite(NRESET, HIGH);
  delay(10);

  I2C_DisableAllChannels(I2C_MUX_ADDRESS);
  
  motorDriverInit(0,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(1,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(2,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(3,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(4,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(5,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(6,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943
  
  motorDriverInit(7,MOTOR_DRIVER_DEFAULT_ADDRESS); // Initialize the motor driver LP3943


  resetEncoder(0);
  resetEncoder(1);
  resetEncoder(2);
  resetEncoder(3);
  resetEncoder(4);
  resetEncoder(5);
  resetEncoder(6);
  resetEncoder(7);
  resetEncoder(8);
  resetEncoder(9);
  resetEncoder(10);
  resetEncoder(11);
  resetEncoder(12);
  resetEncoder(13);
  resetEncoder(14);
  resetEncoder(15);

  SetUpBMS(); // Call the setup function for the BMS
  attachInterrupt(digitalPinToInterrupt(RDY), onBMSReadyRise, RISING);

  setBMSConversionState("CONVERSION_ON");  // To turn conversions ON

  
  delay(100); // Wait for the multiplexer to switch channels
  

  precharge(1); // Precharge with 1 capacitor

}






// Main loop function

void loop() {

  while (Serial.available() > 0) {
        char inChar = Serial.read();
        if (inChar == '\n' || inChar == '\r') {
            inputBuffer[inputPos] = '\0'; // Null-terminate

            // Process the command
            Serial.print("Received command: ");
            Serial.println(inputBuffer);

            // Parse "move" command with 4 arguments: move <mux_channel> <chip_address> <speed> <direction>
            char cmd[8];
            int mux_channel, chip_address, speed, directionInt;
            if (sscanf(inputBuffer, "%s %d %d %d %d", cmd, &mux_channel, &chip_address, &speed, &directionInt) == 5) {
                if (strcmp(cmd, "move") == 0) {
                    bool direction = (directionInt != 0); // Convert int to bool
                    variableMotionControl(mux_channel, chip_address, speed, direction);
                    Serial.print("Moving motor on mux channel ");
                    Serial.print(mux_channel);
                    Serial.print(", chip address ");
                    Serial.print(chip_address, HEX);
                    Serial.print(", speed ");
                    Serial.print(speed);
                    Serial.print(", direction ");
                    Serial.println(direction ? "true" : "false");
                }
            } else if (strcmp(inputBuffer, "abc") == 0) {
                Serial.println("Running test for 'abc'!");
            } else if (strcmp(inputBuffer, "a") == 0) {
                Serial.println("Running test A...");
                // ...test A code...
                packVoltage = readPackSense();
                Serial.print( "\n Pack Voltage: ");
                Serial.println(packVoltage);

                motorDriverRegControl(7, MOTOR_DRIVER_DEFAULT_ADDRESS, false); // Enable the motor driver LP3943 on channel 7
                delay(1000); // Wait for 1 second

                packVoltage = readPackSense();
                Serial.print( "\n Pack Voltage: ");
                Serial.println(packVoltage);

            } else if (strcmp(inputBuffer, "b") == 0) {
                Serial.println("Running test B...");
                // ...test B code...

                packVoltage = readPackSense();
                Serial.print( "\n Pack Voltage: ");
                Serial.println(packVoltage);

                motorDriverRegControl(7, MOTOR_DRIVER_DEFAULT_ADDRESS, true); // Enable the motor driver LP3943 on channel 7
                delay(1000); // Wait for 1 second

                packVoltage = readPackSense();
                Serial.print( "\n Pack Voltage: ");
                Serial.println(packVoltage);

            } else if (strcmp(inputBuffer, "c") == 0) {
                Serial.println("Running test C...");
                // ...test C code...
            } else {
                Serial.println("Unknown command.");
            }

            // Reset buffer
            inputPos = 0;
            inputBuffer[0] = '\0';
        } else if (inputPos < sizeof(inputBuffer) - 1) {
            inputBuffer[inputPos++] = inChar;
        }
    }

}