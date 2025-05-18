// BMS_SetupCommands.cpp
// ---------------------
// Implementation of functions for sending setup and configuration commands to the BMS (Battery Management System) module.
// Handles register mapping, configuration, and identity commands for device initialization and setup.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.


#include "Arduino.h"
#include "BMS_CoreCommands.h"




// Function to send configuration commands to the BMS currently a list format to show each register.
void sendBMSConfigCommand(const char* command) {
    uint8_t registerAddress;
    uint16_t data;

    // Map commands to register addresses and data values
    if (strcmp(command, "CFG2_FILTERS_CYCLES") == 0) {
        registerAddress = 0x04; // Activates what is been sensed and where the fets are.
        data = 0x19FF;          // this activates all sensors and deactivates crc, ovc and sc_en. Fets left in default
    }else if (strcmp(command, "TO_PRDV_BAL_MSK") == 0) {
        registerAddress = 0x13; // These are masking bits. they prevent the device tripping the fets or relevant registers if an error is detected. this will want to be implemented in future.
        data = 0x7FFF;          // set to mask all but the highest bit, this is unused.
    } else if (strcmp(command, "TO_FUSE_RST_MSK") == 0) {
        registerAddress = 0x14; // Mask the actuation of the fuse. these arent in use on the board but could be in future.
        data = 0x007F;          // Activate all but the unused bits. 
    } else if (strcmp(command, "TO_FAULTN_MSK") == 0) {
        registerAddress = 0x15; // Masking the  faultn pin, this is the most likely register to be changed in future as can directly be sensed from the teensy.
        data = 0x0FFF;          // Mask all for now. Last four bits unused
    } else if (strcmp(command, "CURR_MSK") == 0) {
        registerAddress = 0x16; // More masking registers. pertaining to current.
        data = 0x0FFF;          // mask all for now. Last four bits unused
    }  else if (strcmp(command, "DIAG_OV_OT_UT") == 0) {
        registerAddress = 0x2A; // These are all diagnostic flags this command will reset them, can expand on them to be read ata a later date.
        data = 0x0000;          // Set to clear the fault counters and flags
    } else if (strcmp(command, "DIAG_UV") == 0) {
        registerAddress = 0x2B; // Registers that denote under voltage events.
        data = 0x0000;          // Set to clear the fault counters and flags
    } else if (strcmp(command, "DIAG_CURR") == 0) {
        registerAddress = 0x2F; // Diagnostic flags for current events.
        data = 0x0000;          // For now wipes the flags.
    } else {
        Serial.println("Unknown Config command.");
        return;
    }

    //Checks if conversion is active and if so turns it off.
    // This is to ensure that the BMS is not in conversion mode while writing Data.
    if (bmsConversionActive == 1) {
        setBMSConversionState("CONVERSION_OFF");
        
    }
    writeBMSData(0x49, registerAddress, data);

    Serial.print("Config Command sent: ");
    Serial.print(command);
    Serial.print(" with data 0x");
    Serial.println(data, HEX);
    delay(10); // Add a delay to ensure the command is processed
}


void RWBMSNVM(const char* command) {
    uint8_t registerAddress;
    uint16_t data;

    if (strcmp(command, "NVM_2_DL") == 0) {
        registerAddress = 0x20; // THIS COMMAND CAN ONLY BE ISSUED 32 TIMES!!! This is the command to commit the changes to the NVM FROM I2C
        data = 0xAAAA;          // NEVER PUT THIS IN THE MAIN LOOP, SETUP ONLY!
    } else if (strcmp(command, "NVM_2_UL") == 0) {
        registerAddress = 0x20; // This is the command to commit the DATA from the NVM TO I2C, this should be called on startup.
        data = 0x5555;          // This is the command as seen in the datasheets.
    } else {
        Serial.println("Unknown NVM command.");
        return;
    }

    //Checks if conversion is active and if so turns it off.
    if (bmsConversionActive == 1) {
        setBMSConversionState("CONVERSION_OFF");
    }
    writeBMSData(0x49, registerAddress, data);

    Serial.print("NVM Command sent: ");
    Serial.print(command);
    Serial.print(" with data 0x");
    Serial.println(data, HEX);
    delay(100); // Add a delay to ensure the command is processed
}

//Function to assign identity and and other unique information to the bms chip. these have no bearing on performance so are ideal to test commands.
//Function to assign identity and and other unique information to the bms chip. these have no bearing on performance so are ideal to test commands.
void sendBMSIdentityCommand(const char* command, uint16_t data) {
    uint8_t registerAddress;
    // Statement to prevent the identity value from being 0x0000. this could be used to check NVM is been used.
    if (data == 0x0000) {
        Serial.print("Error: Identity value for ");
        Serial.print(command);
        Serial.println(" cannot be 0x0000.");
        return;
    }

    if (strcmp(command, "MANUFACTURE_NAME_MSB") == 0) {
        registerAddress = 0x17;
    } else if (strcmp(command, "MANUFACTURE_NAME_LSB") == 0) {
        registerAddress = 0x18;
    } else if (strcmp(command, "MANUFACTURING_DATE") == 0) {
        registerAddress = 0x19;
    } else if (strcmp(command, "FIRST_USAGE_DATE") == 0) {
        registerAddress = 0x1A;
    } else if (strcmp(command, "SERIAL_NUMBER_MSB") == 0) {
        registerAddress = 0x1B;
    } else if (strcmp(command, "SERIAL_NUMBER_LSB") == 0) {
        registerAddress = 0x1C;
    } else if (strcmp(command, "DEVICE_NAME_MSB") == 0) {
        registerAddress = 0x1D;
    } else if (strcmp(command, "DEVICE_NAME_LSB") == 0) {
        registerAddress = 0x1E;
    } else {
        Serial.println("Unknown identity command.");
        return;
    }

    //Checks if conversion is active and if so turns it off.
    // This is to ensure that the BMS is not in conversion mode while writing identity data.
    if (bmsConversionActive == 1) {
        setBMSConversionState("CONVERSION_OFF");
    }
    writeBMSData(0x49, registerAddress, data);

    Serial.print("Identity Command sent: ");
    Serial.print(command);
    Serial.print(" with data 0x");
    Serial.println(data, HEX);
}