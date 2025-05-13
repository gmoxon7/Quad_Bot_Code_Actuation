//BMS_I2C_FCT.cpp
// This file contains the implementation of I2C functions for reading and writing data to a BMS module

#include "PinAssignments.h"
#include "BMS_I2C_FCT.h" // Include the header file for BMS I2C functions





void writeBMSData(uint8_t chipAddress, uint8_t registerAddress, uint16_t data) {
    // Step 1: Start communication with the slave device
    Wire1.beginTransmission(chipAddress);

    // Step 2: Send the register address
    Wire1.write(registerAddress);

    // Step 3: Send the two data bytes (MSB first, then LSB)
    uint8_t highByte = (data >> 8) & 0xFF; // Extract the most significant byte
    uint8_t lowByte = data & 0xFF;        // Extract the least significant byte
    Wire1.write(highByte);                // Send the MSB
    Wire1.write(lowByte);                 // Send the LSB

    // Step 4: End the transmission (send a stop condition)
    if (Wire1.endTransmission(true) != 0) { // Check for errors
        Serial.println("Error: Failed to write data to BMS.");
    } else {
        Serial.println("Write successful.");
    }
}


// Single Read Function for L9961
uint16_t readBMSData(uint8_t chipAddress, uint8_t registerAddress) {
    uint16_t data = 0;

    // Step 1: Send the slave address with the write bit and the register address
    Wire1.beginTransmission(chipAddress);
    Wire1.write(registerAddress);
    Wire1.endTransmission(true); // Send a repeated start condition
    delayMicroseconds(3); // Small delay to allow the device to process the request (tbuf)
    // Step 2: Send the slave address with the read bit
    Wire1.requestFrom(chipAddress, (uint8_t)2, true); // Request 2 bytes from the slave

    // Step 3: Read the two bytes of data
    if (Wire1.available() >= 2) {
        uint8_t highByte = Wire1.read(); // Read the first byte (MSB)
        uint8_t lowByte = Wire1.read();  // Read the second byte (LSB)
        data = (highByte << 8) | lowByte; // Combine the two bytes into a 16-bit value
    }

    return data;
}




void sendBMSCommand(const char* command) {
    uint8_t registerAddress;
    uint16_t data;

    // Map commands to register addresses and data values
    if (strcmp(command, "BAL_ENABLE") == 0) {
        registerAddress = 0x01; // Register address to interface with balancing fets
        data = 0x1F;          // set balancing on
    } else if (strcmp(command, "BAL_DISABLE") == 0) {
        registerAddress = 0x01; // Register address to interface with balancing fets
        data = 0x0000;          // Set balancing off
    } else if (strcmp(command, "CONVERSION_OFF") == 0) {
        registerAddress = 0x02; // Register address for CFG1_FILTERS_CYCLES
        data = 0x0000;          // Dont configure while performing conversions, deactivates conversions.
    } else if (strcmp(command, "CFG1_FILTERS_CYCLES") == 0) {
        registerAddress = 0x02; // register address for CFG1_FILTERS_CYCLES
        data = 0x0FFF;          // sets the registers to maximum time periods, performs conv every 310ms
    } else if (strcmp(command, "CFG2_FILTERS_CYCLES") == 0) {
        registerAddress = 0x04; // Activates what is been sensed and where the fets are.
        data = 0x19FF;          // this activates all sensors and deactivates crc, ovc and sc_en. Fets left in default
    } else if (strcmp(command, "CSA_GAIN_FACTOR") == 0) {
        registerAddress = 0x05; // this is a gain factor for the Current sense amplifier.
        data = 0x8000;          // ive left it as the default.
    } else if (strcmp(command, "VCELL_OV_TH") == 0) {
        registerAddress = 0x06; // Sets the cell voltage thresholds for overvoltage. and the number of times its allowed.
        data = 0x0FE1;          // set the number of times allowed to saturation and set the threshold to 0xE1=225=1110 0001. 4.4=16*1.22mV*CELL_OV_TH
    } else if (strcmp(command, "VCELL_UV_TH") == 0) {
        registerAddress = 0x07; // Sets the cell voltage thresholds for undervoltage. and the number of times its allowed.
        data = 0x0F70;          // as above but set for 2.2V 
    } else if (strcmp(command, "VCELL_SEVERE_DELTA_THRS") == 0) {
        registerAddress = 0x08; // register for setting the severe delta threshold if severe the protections trigger.
        data = 0x0A0A;          // Set 0.2V either side of the UV and OV. NO PROTECTIONS IN PLACE ANYWAY.
    } else if (strcmp(command, "VCELL_BAL_UV_DELTA_TH") == 0) {
        registerAddress = 0x09; // This reg defines behaviour of balancing when undervoltage occurs.
        data = 0x0F00;          // set so maximum number of counts. threshold change set to 0
    } else if (strcmp(command, "VB_OV_TH") == 0) {
        registerAddress = 0x0A; // Register sets the overvoltage threshold for the VB pin. and the number of times its allowed.
        data = 0x0FEB;          // Set the times allowed to saturation, set the trip point to 23v EQ =23=16*6.1mV*1110 1011
    } else if (strcmp(command, "VB_UV_TH") == 0) {
        registerAddress = 0x0B; // Example register for disabling a feature
        data = 0x0F70;          // Example data to disable the feature 
    } else if (strcmp(command, "VB_SUM_MAX_DIFF_TH") == 0) {
        registerAddress = 0x0C; // Plausability check threshold for cell sum and vb readings.
        data = 0x0014;          // Doesnt give an equation for this one so im going to set it to 2v as if ff=25v. 
    } else if (strcmp(command, "VNTC_OT_TH") == 0) {
        registerAddress = 0x0D; // Sets the upper threshold for temperature error events.
        data = 0xFFFF;          // Set with ntc resolution 0.806mV THIS FULL REGISTER ONLY CLOCKS AN OVERHEAT CASE IF THE PIN = VREG.
    } else if (strcmp(command, "VNTC_UT_TH") == 0) {
        registerAddress = 0x0E; // Opposite to above. Sets the lower threshold for temperature error events.
        data = 0x0000;          // this is set to zero to ensure no events trigger 
    } else if (strcmp(command, "VNTC_SEVERE_OT_DELTA_TH") == 0) {
        registerAddress = 0x0F; // Negative delta for the severe overtemperature threshold.
        data = 0x0000;          // this is set to zero to ensure no events trigger
    } else if (strcmp(command, "OVC_THRESHOLDS") == 0) {
        registerAddress = 0x10; // detects overcurrent events with formula 128*V_CUR_RES*CODE (CUR_RES=Vdiff_range/2^nBit_CSA-1)
        data = 0xFFFF;          // Maxed to read maximum in both direction to trip error as not used.
    } else if (strcmp(command, "PERSISTENT_OVC_THRESHOLDS") == 0) {
        registerAddress = 0x11; // another over current check, this differs from above in that it has a time factor attached, the threshold is exceeded four 4scans before been tripped.
        data = 0x00FF;          // set to maximum to avoid tripping.
    } else if (strcmp(command, "SC_THRESHOLD") == 0) {
        registerAddress = 0x12; // This is a voltage seen across the shunt resistor. This is used to detect short circuit events.
        data = 0x00FF;          // eq= 49.14mV+14.04mV*CODE. This is set to max to avoid tripping.
    } else if (strcmp(command, "TO_PRDV_BAL_MSK") == 0) {
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
    } else if (strcmp(command, "MANUFACTURE_NAME_MSB") == 0) {
        registerAddress = 0x17; // Not super informative for now will be used to make sure registers are been written to as can easily be checked without changing behaviour.
        data = 0xFFFF;          // opposite the default.
    } else if (strcmp(command, "MANUFACTURE_NAME_LSB") == 0) {
        registerAddress = 0x18; // as above
        data = 0xFFFF;          // set all high as is opposite the default
    } else if (strcmp(command, "MANUFACTURING_DATE") == 0) {
        registerAddress = 0x19; // Mnufacture date, usefulr for batches.
        data = 0xFFFF;          // full register to be set to opposite the default. 
    } else if (strcmp(command, "FIRST_USAGE_DATE") == 0) {
        registerAddress = 0x01A; // Not very informative but could have its uses in future.
        data = 0xFFFF;          // full register to be set to opposite the default.
    } else if (strcmp(command, "SERIAL_NUMBER_MSB") == 0) {
        registerAddress = 0x1B; // again, useful for batches.
        data = 0xFFFF;          // full register to be set to opposite the default.
    } else if (strcmp(command, "SERIAL_NUMBER_LSB") == 0) {
        registerAddress = 0x1C; // again, useful for batches.
        data = 0xFFFF;          // full register to be set to opposite the default.
    } else if (strcmp(command, "DEVICE_NAME_MSB") == 0) {
        registerAddress = 0x1D; // useful for batches.
        data = 0xFFFF;          // for now set to opposite the default.
    } else if (strcmp(command, "DEVICE_NAME_LSB") == 0) {
        registerAddress = 0x1E; // useful for batches.
        data = 0xFFFF;          // set to opposite the default.
    } else if (strcmp(command, "NVM_2_DL") == 0) {
        registerAddress = 0x20; // THIS COMMAND CAN ONLY BE ISSUED 32 TIMES!!! This is the command to commit the changes to the NVM FROM I2C
        data = 0xAAAA;          // This is the 
    } else if (strcmp(command, "NVM_2_UL") == 0) {
        registerAddress = 0x20; // This is the command to commit the DATA from the NVM TO I2C, this should be called on startup.
        data = 0x5555;          // This is the command as seen in the datasheets.
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
    } else if (strcmp(command, "DIAG_OV_OT_UT") == 0) {
        registerAddress = 0x2A; // These are all diagnostic flags this command will reset them, can expand on them to be read ata a later date.
        data = 0x0000;          // Set to clear the fault counters and flags
    } else if (strcmp(command, "DIAG_UV") == 0) {
        registerAddress = 0x2B; // Registers that denote under voltage events.
        data = 0x0000;          // Set to clear the fault counters and flags
    } else if (strcmp(command, "DIAG_CURR") == 0) {
        registerAddress = 0x2F; // Diagnostic flags for current events.
        data = 0x0000;          // For now wipes the flags.
    } else {
        Serial.println("Unknown command.");
        return;
    }

    // Send the command to the BMS
    writeBMSData(0x49, registerAddress, data);
    Serial.print("Command sent: ");
    Serial.println(command);
}