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



#include <Wire.h>


#include <SPI.h> // for the rotary encoder

//Definitions
#define WIRE Wire // for teensy 4.0 this is the i2c library on pins 18(sda) and 19(scl)
#define WIRE Wire1 // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(scl1)


uint8_t I2C_MUX_ADDRESS = 0x70; //I2C address of the multiplexer write version increment for read version
float degrees = 0; // Variable to store the encoder position

uint8_t chipAddress = 0x49; // Replace with the actual I2C address of the L9961
uint8_t registerAddress = 0x0; // checking vb value.

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
}






void loop() {
    // Example: Switch to channel 5
    //I2C_SelectChannel(0x70, 4);
    //delay(2000);
  

    //Serial.print("\nI2C Multiplexer Channels: ");
    //Serial.print(I2C_RD(0x70));

    // Example: Disable all channels
    //I2C_DisableAllChannels(0x70);
    


  
  //testing the SPI encoder 
  //Read position from encoder on channel 0


  // delay(3000); // Wait for 3 second


  //   degrees = readEncoderPosition(0);
  //   Serial.print("Encoder position on channel 0: ");
  //   Serial.println(degrees);

  //   delay(1000); // Wait for 5 second
  

  //   // Test turns counting on channel 0
  //   int16_t turns = readTurns(0); // Read the number of turns on channel 0
  //   Serial.print("Number of turns on channel 0: ");
  //   Serial.println(turns);

  //   delay(1000); // Wait for 5 second
  
  // float packVoltage = readPackSense();
  // Serial.print( "\n Pack Voltage: ");
  // Serial.println(packVoltage);

  // precharge(1); // Precharge with 1 capacitor


  // // Turn off the CHG pin
  // setCHG(false);

  // delay(5000); // Wait for 5 second
  // Serial.print( "\n DSG pin state: OFF ");
  // setDSG(LOW); // Turn off the DSG pin
  // delay(1000); // Wait for 5 second

 


  // uint16_t mfgDate = readManufacturerDate();
  // Serial.print("Manufacturer Date: 0x");
  // Serial.println(mfgDate, HEX);
  
  
  // delay(10); // Wait for the conversion to stabilize



  // // Read and print the cell sum voltage only if data is valid
  // if (isBMSDataValid()) {
  //   float cellSum = readVCellSum();
  //   Serial.print("BMS CellSum: ");
  //   Serial.println(cellSum, 3); // Print with 3 decimal places
  //   bmsDataReady = false; // Mark data as read

  // } else {
  //   Serial.println("BMS data expired, waiting for next conversion...");

  float current = readCurrent();
  Serial.print("BMS Current: ");
  Serial.println(current, 3); // Print with 3 decimal places



  // Read and print the coulomb counter result
    CoulombCountResult cc = readCoulombCounter();
    Serial.print("Coulombs: ");
    Serial.println(cc.coulombs, 6); // Print with 6 decimal places
    Serial.print("Sample Count: ");
    Serial.println(cc.sampleCount);

    delay(6000); // Adjust delay as needed
    
  // }






  
  

 }

