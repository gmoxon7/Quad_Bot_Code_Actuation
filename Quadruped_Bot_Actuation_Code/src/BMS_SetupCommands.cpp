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
void sendBMSConfigCommand(const char* command, const char* valueStr) {
    uint8_t registerAddress;
    uint16_t defaultData = 0;
    uint16_t maxData = 0b1111111111111111; // Default max, will be set per register

    // Map commands to register addresses, default values, and max values (all in binary)
    if (strcmp(command, "CFG1_FILTERS_CYCLES") == 0) {
        registerAddress = 0x04;
        defaultData = 0b0001100111111111; // 0x19FF
        maxData    = 0b0000111111111111; // 0x0FFF
    } else if (strcmp(command, "TO_PRDV_BAL_MSK") == 0) {
        registerAddress = 0x13;
        defaultData = 0b0111111111111111; // 0x7FFF
        maxData    = 0b0111111111111111; // 0x7FFF
    } else if (strcmp(command, "TO_FUSE_RST_MSK") == 0) {
        registerAddress = 0x14;
        defaultData = 0b0000000001111111; // 0x007F
        maxData    = 0b0000000001111111; // 0x007F
    } else if (strcmp(command, "TO_FAULTN_MSK") == 0) {
        registerAddress = 0x15;
        defaultData = 0b0000111111111111; // 0x0FFF
        maxData    = 0b0000111111111111; // 0x0FFF
    } else if (strcmp(command, "CURR_MSK") == 0) {
        registerAddress = 0x16;
        defaultData = 0b0000111111111111; // 0x0FFF
        maxData    = 0b0000111111111111; // 0x0FFF
    } else if (strcmp(command, "DIAG_OV_OT_UT") == 0) {
        registerAddress = 0x2A;
        defaultData = 0b0000000000000000; // 0x0000
        maxData    = 0b1111111111111111; // 0xFFFF
    } else if (strcmp(command, "DIAG_UV") == 0) {
        registerAddress = 0x2B;
        defaultData = 0b0000000000000000; // 0x0000
        maxData    = 0b1111111111111111; // 0xFFFF
    } else if (strcmp(command, "DIAG_CURR") == 0) {
        registerAddress = 0x2F;
        defaultData = 0b0000000000000000; // 0x0000
        maxData    = 0b1111111111111111; // 0xFFFF
    } else {
        Serial.println("Unknown Config command.");
        return;
    }

    uint16_t data = 0;
    if (strcmp(valueStr, "default") == 0) {
        data = defaultData;
    } else if (strncmp(valueStr, "0b", 2) == 0) {
        data = (uint16_t)strtol(valueStr + 2, nullptr, 2);
        if (data > maxData) {
            Serial.print("Warning: Value ");
            Serial.print(valueStr);
            Serial.print(" exceeds max allowed for ");
            Serial.print(command);
            Serial.print(". Capping to 0b");
            for (int i = 15; i >= 0; --i) Serial.print((maxData >> i) & 1);
            Serial.println();
            data = maxData;
        }
    } else {
        Serial.println("Error: Value must be \"default\" or a binary string like \"0b101010\".");
        return;
    }

    //Checks if conversion is active and if so turns it off.
    if (bmsConversionActive == 1) {
        setBMSConversionState("CONVERSION_OFF");
    }
    writeBMSData(0x49, registerAddress, data);

    Serial.print("Config Command sent: ");
    Serial.print(command);
    Serial.print(" with data 0x");
    Serial.print(data, HEX);
    Serial.print(" (binary: ");
    for (int i = 15; i >= 0; --i) Serial.print((data >> i) & 1);
    Serial.println(")");
    delay(10);
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


void sendBMSRealTimeCommand(const char* command) {
    uint8_t registerAddress;
    uint16_t data;

    // Map commands to register addresses and data values
    if (strcmp(command, "BAL_ENABLE") == 0) {
        registerAddress = 0x01; // Register address to interface with balancing fets
        data = 0x001F;          // set balancing on
    } else if (strcmp(command, "BAL_DISABLE") == 0) {
        registerAddress = 0x01; // Register address to interface with balancing fets
        data = 0x0000;          // Set balancing off
    } else if (strcmp(command, "GO2SHIP") == 0) {
        registerAddress = 0x21; // Sends the device to ship mode. this is a low power mode.
        data = 0x2000;          // Sends the 10 to 13th-14th bit note as with all write commands sets the read only registers zero, erasing the information.
    } else if (strcmp(command, "GO2STBY") == 0) {
        registerAddress = 0x22; // Sends the chip to standby mode. this is a low power mode.
        data = 0x2000;          // sets the 10 to 13th-14th bit note as with all write commands sets the read only registers zero, erasing the information.
    } else if (strcmp(command, "FUSE_TRIG_DISARM") == 0) {
        registerAddress = 0x23; // turns off the fuse
        data = 0x1000;          // 
    } else if (strcmp(command, "FUSE_TRIG_ARM") == 0) {
        registerAddress = 0x23; // turns on the fuse
        data = 0x2000;          // set all high as is opposite the default
    } else if (strcmp(command, "FUSE_TRIG_FIRE_INTERRUPT") == 0) {
        registerAddress = 0x24; // Stops the fuse from firing
        data = 0x1000;          // set to 1 on the 13th bit to stop the fuse from firing.
    } else if (strcmp(command, "FUSE_TRIG_FIRE") == 0) {
        registerAddress = 0x24; // A read/write bit that can be actuated by the BMS or the MCU.
        data = 0x2000;          // this will fire on command.
    }  else {
        Serial.println("Unknown Real Time command.");
        return;
    }

    //Checks if conversion is active and if so turns it off.
    // This is to ensure that the BMS is not in conversion mode while writing configuration data.
    if (bmsConversionActive == 1) {
        setBMSConversionState("CONVERSION_OFF");
        
    }
    writeBMSData(0x49, registerAddress, data);

    Serial.print("BMS Command sent: ");
    Serial.print(command);
    Serial.print(" with data 0x");
    Serial.println(data, HEX);
}



//eventually the BAL_enable command needs to be modified so it can operate with the chg and discharge fets.