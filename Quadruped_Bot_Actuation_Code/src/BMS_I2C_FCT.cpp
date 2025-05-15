//BMS_I2C_FCT.cpp
// This file contains the implementation of I2C functions for reading and writing data to a BMS module

#include "PinAssignments.h"
#include "BMS_I2C_FCT.h" // Include the header file for BMS I2C functions


int bmsConversionActive = 0; // 1 = ON, 0 = OFF This variable is used in the config and identity functions to keep track of if the measuring registers are changing.

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


// Function to write data to the BMS module
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




//function used whenever a configuration is set. this stops registers changing while been written.
void setBMSConversionState(const char* state) {
    if (strcmp(state, "CONVERSION_OFF") == 0) {
        writeBMSData(0x49, 0x02, 0x0000); // Conversion off: register 0x02, data 0x0000
        bmsConversionActive = 0;
        Serial.println("Conversion OFF command sent.");
    } else if (strcmp(state, "CONVERSION_ON") == 0) {
        writeBMSData(0x49, 0x02, 0x0FFF); // Conversion on: register 0x02, data 0x0FFF conversions occur every 310ms
        bmsConversionActive = 1;
        Serial.println("Conversion ON command sent.");
    } else {
        Serial.println("Unknown conversion state command.");
    }
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

void sendBMSNumericalCommand(const char* command, const char* arg1, const char* arg2 = nullptr) {

//when passing arguments pass the command, the 8lsb argumennt, and the 8msb argument.

    uint8_t registerAddress;
    uint16_t data;

    // Helper lambda to convert "max"/"min" or string to value
    auto parseValue = [](const char* arg, uint16_t minVal, uint16_t maxVal) -> uint16_t {
        if (!arg) return 0;
        if (strcasecmp(arg, "max") == 0) return maxVal;
        if (strcasecmp(arg, "min") == 0) return minVal;
        long val = strtol(arg, nullptr, 10);
        if (val < minVal) return minVal;
        if (val > maxVal) return maxVal;
        return (uint16_t)val;
    };

    // Map commands to register addresses and value ranges
    if (strcmp(command, "CSA_GAIN_FACTOR") == 0) {
        registerAddress = 0x05;
        value1 = parseValue(arg1, 0x0000, 0xFFFF);
    }
    
    
    else if (strcmp(command, "VCELL_OV_TH") == 0) {
        registerAddress = 0x06;
    // Hardcoded min/max for this register
    const float VCELL_OV_TH_MIN = 4.3f;
    const float VCELL_OV_TH_MAX = 5.0f;
    const int NCELL_OV_CNT_TH_MIN = 3;
    const int NCELL_OV_CNT_TH_MAX = 15;
    const float VCELL_RES = 0.00122f; // Example value, check your datasheet

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VCELL_OV_TH_MIN) voltage = VCELL_OV_TH_MIN;
    if (voltage > VCELL_OV_TH_MAX) voltage = VCELL_OV_TH_MAX;
    int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
    if (vcell_code < 0) vcell_code = 0;
    if (vcell_code > 0xFF) vcell_code = 0xFF;

    // Parse and clamp count argument
    int ncell_ovcnt = atoi(arg2 ? arg2 : "0");
    if (ncell_ovcnt < NCELL_OV_CNT_TH_MIN) ncell_ovcnt = NCELL_OV_CNT_TH_MIN;
    if (ncell_ovcnt > NCELL_OV_CNT_TH_MAX) ncell_ovcnt = NCELL_OV_CNT_TH_MAX;

    // Pack the data: [0000][NCELL_OV_CNT_TH(4)][VCELL_OV_TH(8)]
    uint16_t data = ((ncell_ovcnt & 0xF) << 8) | (vcell_code & 0xFF);

    } 
    
    
    else if (strcmp(command, "VCELL_UV_TH") == 0) {
    registerAddress = 0x07;
    // Hardcoded min/max for this register
    const float VCELL_UV_TH_MIN = 2.0f;
    const float VCELL_UV_TH_MAX = 3.0f;
    const int NCELL_UV_CNT_TH_MIN = 3;
    const int NCELL_UV_CNT_TH_MAX = 15;
    const float VCELL_RES = 0.00122f; // Example value, check your datasheet

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VCELL_UV_TH_MIN) voltage = VCELL_UV_TH_MIN;
    if (voltage > VCELL_UV_TH_MAX) voltage = VCELL_UV_TH_MAX;
    int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
    if (vcell_code < 0) vcell_code = 0;
    if (vcell_code > 0xFF) vcell_code = 0xFF;

    // Parse and clamp count argument
    int ncell_uvcnt = atoi(arg2 ? arg2 : "0");
    if (ncell_uvcnt < NCELL_UV_CNT_TH_MIN) ncell_uvcnt = NCELL_UV_CNT_TH_MIN;
    if (ncell_uvcnt > NCELL_UV_CNT_TH_MAX) ncell_uvcnt = NCELL_UV_CNT_TH_MAX;

    // Pack the data: [0000][NCELL_UV_CNT_TH(4)][VCELL_UV_TH(8)]
    uint16_t data = ((ncell_uvcnt & 0xF) << 8) | (vcell_code & 0xFF);

    }


    else if (strcmp(command, "VCELL_SEVERE_DELTA_THRS") == 0) {
    registerAddress = 0x08;
    // Hardcoded min/max for this register
    const float VCELL_SEVERE_DELTA_TH_MIN = 2.0f;
    const float VCELL_SEVERE_DELTA_TH_MAX = 5.0f;
    const float VCELL_RES = 0.00122f; // Example value, check your datasheet

    // Parse and clamp OV delta argument (arg1)
    float ov_voltage = atof(arg1);
    if (ov_voltage < VCELL_SEVERE_DELTA_TH_MIN) ov_voltage = VCELL_SEVERE_DELTA_TH_MIN;
    if (ov_voltage > VCELL_SEVERE_DELTA_TH_MAX) ov_voltage = VCELL_SEVERE_DELTA_TH_MAX;
    int ov_code = (int)((ov_voltage / (16 * VCELL_RES)) + 0.5f);
    if (ov_code < 0) ov_code = 0;
    if (ov_code > 0xFF) ov_code = 0xFF;

    // Parse and clamp UV delta argument (arg2)
    float uv_voltage = atof(arg2 ? arg2 : "0");
    if (uv_voltage < VCELL_SEVERE_DELTA_TH_MIN) uv_voltage = VCELL_SEVERE_DELTA_TH_MIN;
    if (uv_voltage > VCELL_SEVERE_DELTA_TH_MAX) uv_voltage = VCELL_SEVERE_DELTA_TH_MAX;
    int uv_code = (int)((uv_voltage / (16 * VCELL_RES)) + 0.5f);
    if (uv_code < 0) uv_code = 0;
    if (uv_code > 0xFF) uv_code = 0xFF;

    // Pack the data: [OV_CODE(8)][UV_CODE(8)]
    uint16_t data = ((uv_code & 0xFF) << 8) | (ov_code & 0xFF);

    }


    else if (strcmp(command, "VCELL_BAL_UV_DELTA_TH") == 0) {
    registerAddress = 0x09;
    // Hardcoded min/max for this register (adjust as per your datasheet)
    const float VCELL_BAL_UV_DELTA_TH_MIN = 2.0f;
    const float VCELL_BAL_UV_DELTA_TH_MAX = 5.0f;
    const int NCELL_BAL_UV_CNT_TH_MIN = 3;
    const int NCELL_BAL_UV_CNT_TH_MAX = 15;
    const float VCELL_RES = 0.00122f; // Example value, check your datasheet

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VCELL_BAL_UV_DELTA_TH_MIN) voltage = VCELL_BAL_UV_DELTA_TH_MIN;
    if (voltage > VCELL_BAL_UV_DELTA_TH_MAX) voltage = VCELL_BAL_UV_DELTA_TH_MAX;
    int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
    if (vcell_code < 0) vcell_code = 0;
    if (vcell_code > 0xFF) vcell_code = 0xFF;

    // Parse and clamp count argument
    int ncell_cnt = atoi(arg2 ? arg2 : "0");
    if (ncell_cnt < NCELL_BAL_UV_CNT_TH_MIN) ncell_cnt = NCELL_BAL_UV_CNT_TH_MIN;
    if (ncell_cnt > NCELL_BAL_UV_CNT_TH_MAX) ncell_cnt = NCELL_BAL_UV_CNT_TH_MAX;

    // Pack the data: [0000][NCELL_BAL_UV_CNT_TH(4)][VCELL_BAL_UV_DELTA_TH(8)]
    uint16_t data = ((ncell_cnt & 0xF) << 8) | (vcell_code & 0xFF);

    } 


    else if (strcmp(command, "VB_OV_TH") == 0) {
    registerAddress = 0x0A;
    const float VB_OV_TH_MIN = 0.0f;
    const float VB_OV_TH_MAX = 25.0f;
    const int NCELL_OV_CNT_TH_MIN = 0;
    const int NCELL_OV_CNT_TH_MAX = 15;
    const float VB_RES = 0.0061f; // 6.1mV

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VB_OV_TH_MIN) voltage = VB_OV_TH_MIN;
    if (voltage > VB_OV_TH_MAX) voltage = VB_OV_TH_MAX;
    int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
    if (vb_code < 0) vb_code = 0;
    if (vb_code > 0xFF) vb_code = 0xFF;

    // Parse and clamp count argument
    int ncell_ovcnt = atoi(arg2 ? arg2 : "0");
    if (ncell_ovcnt < NCELL_OV_CNT_TH_MIN) ncell_ovcnt = NCELL_OV_CNT_TH_MIN;
    if (ncell_ovcnt > NCELL_OV_CNT_TH_MAX) ncell_ovcnt = NCELL_OV_CNT_TH_MAX;

    // Pack the data: [0000][NCELL_OV_CNT_TH(4)][VB_OV_TH(8)]
    uint16_t data = ((ncell_ovcnt & 0xF) << 8) | (vb_code & 0xFF);


    }


    else if (strcmp(command, "VB_UV_TH") == 0) {
    registerAddress = 0x0B;
    const float VB_RES = 0.0061f; // 6.1mV
    const float VB_UV_TH_MIN = 0.0f;
    const float VB_UV_TH_MAX = 25.0f;
    const int NCELL_UV_CNT_TH_MIN = 0;
    const int NCELL_UV_CNT_TH_MAX = 15;
    

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VB_UV_TH_MIN) voltage = VB_UV_TH_MIN;
    if (voltage > VB_UV_TH_MAX) voltage = VB_UV_TH_MAX;
    int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
    if (vb_code < 0) vb_code = 0;
    if (vb_code > 0xFF) vb_code = 0xFF;

    // Parse and clamp count argument
    int ncell_uvcnt = atoi(arg2 ? arg2 : "0");
    if (ncell_uvcnt < NCELL_UV_CNT_TH_MIN) ncell_uvcnt = NCELL_UV_CNT_TH_MIN;
    if (ncell_uvcnt > NCELL_UV_CNT_TH_MAX) ncell_uvcnt = NCELL_UV_CNT_TH_MAX;

    // Pack the data: [0000][NCELL_UV_CNT_TH(4)][VB_UV_TH(8)]
    uint16_t data = ((ncell_uvcnt & 0xF) << 8) | (vb_code & 0xFF);
    

    }


    else if (strcmp(command, "VB_SUM_MAX_DIFF_TH") == 0) {
    registerAddress = 0x0C;
    const float VB_SUM_MAX_DIFF_TH_MIN = 0.0f;
    const float VB_SUM_MAX_DIFF_TH_MAX = 25.0f;
    const float VB_RES = 0.0061f; // 6.1mV

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VB_SUM_MAX_DIFF_TH_MIN) voltage = VB_SUM_MAX_DIFF_TH_MIN;
    if (voltage > VB_SUM_MAX_DIFF_TH_MAX) voltage = VB_SUM_MAX_DIFF_TH_MAX;
    int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
    if (vb_code < 0) vb_code = 0;
    if (vb_code > 0xFF) vb_code = 0xFF;

    // Pack the data: [00000000][VB_SUM_MAX_DIFF_TH(8)]
    uint16_t data = vb_code & 0xFF;


     }


    else if (strcmp(command, "VNTC_OT_TH") == 0) {
    registerAddress = 0x0D;
    const float VNTC_OT_TH_MIN = 0.2f;
    const float VNTC_OT_TH_MAX = 3.3f;
    const int NNTC_OT_CNT_TH_MIN = 0;
    const int NNTC_OT_CNT_TH_MAX = 15;
    const float VNTC_RES = 0.806f;

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VNTC_OT_TH_MIN) voltage = VNTC_OT_TH_MIN;
    if (voltage > VNTC_OT_TH_MAX) voltage = VNTC_OT_TH_MAX;
    int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
    if (ntc_code < 0) ntc_code = 0;
    if (ntc_code > 0xFFF) ntc_code = 0xFFF;

    // Parse and clamp count argument
    int nntc_cnt = atoi(arg2 ? arg2 : "0");
    if (nntc_cnt < NNTC_OT_CNT_TH_MIN) nntc_cnt = NNTC_OT_CNT_TH_MIN;
    if (nntc_cnt > NNTC_OT_CNT_TH_MAX) nntc_cnt = NNTC_OT_CNT_TH_MAX;

    // Pack the data: [NNTC_OT_CNT_TH(4)][NTC_OT_TH(12)]
    uint16_t data = ((nntc_cnt & 0xF) << 12) | (ntc_code & 0xFFF);
    

    }



    else if (strcmp(command, "VNTC_UT_TH") == 0) {
    registerAddress = 0x0E;
    const float VNTC_UT_TH_MIN = 0.2f;
    const float VNTC_UT_TH_MAX = 3.3f;
    const int NNTC_UT_CNT_TH_MIN = 0;
    const int NNTC_UT_CNT_TH_MAX = 15;
    const float VNTC_RES = 0.806f;

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VNTC_UT_TH_MIN) voltage = VNTC_UT_TH_MIN;
    if (voltage > VNTC_UT_TH_MAX) voltage = VNTC_UT_TH_MAX;
    int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
    if (ntc_code < 0) ntc_code = 0;
    if (ntc_code > 0xFFF) ntc_code = 0xFFF;

    // Parse and clamp count argument
    int nntc_cnt = atoi(arg2 ? arg2 : "0");
    if (nntc_cnt < NNTC_UT_CNT_TH_MIN) nntc_cnt = NNTC_UT_CNT_TH_MIN;
    if (nntc_cnt > NNTC_UT_CNT_TH_MAX) nntc_cnt = NNTC_UT_CNT_TH_MAX;

    // Pack the data: [NNTC_UT_CNT_TH(4)][NTC_UT_TH(12)]
    uint16_t data = ((nntc_cnt & 0xF) << 12) | (ntc_code & 0xFFF);
    

    }


    else if (strcmp(command, "VNTC_SEVERE_OT_DELTA_TH") == 0) {
    registerAddress = 0x0F;
    const float VNTC_SEVERE_OT_DELTA_TH_MIN = 0.2f;
    const float VNTC_SEVERE_OT_DELTA_TH_MAX = 3.3f;
    const float VNTC_RES = 0.806f;

    // Parse and clamp voltage argument
    float voltage = atof(arg1);
    if (voltage < VNTC_SEVERE_OT_DELTA_TH_MIN) voltage = VNTC_SEVERE_OT_DELTA_TH_MIN;
    if (voltage > VNTC_SEVERE_OT_DELTA_TH_MAX) voltage = VNTC_SEVERE_OT_DELTA_TH_MAX;
    int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
    if (ntc_code < 0) ntc_code = 0;
    if (ntc_code > 0xFFF) ntc_code = 0xFFF;

    // Pack the data: [0000][NTC_SEVERE_OT_DELTA_TH(12)]
    uint16_t data = ntc_code & 0xFFF;

    
    }



    } else if (strcmp(command, "OVC_THRESHOLDS") == 0) {
        registerAddress = 0x10;
        value1 = parseValue(arg1, 0x0000, 0xFFFF);
    } else if (strcmp(command, "PERSISTENT_OVC_THRESHOLDS") == 0) {
        registerAddress = 0x11;
        value1 = parseValue(arg1, 0x0000, 0xFFFF);
    } else if (strcmp(command, "SC_THRESHOLD") == 0) {
        registerAddress = 0x12;
        value1 = parseValue(arg1, 0x0000, 0xFFFF);
    } else {
        Serial.println("Unknown numerical command.");
        return;
    }

   
    // Conversion state check
    if (bmsConversionActive == 1) {
        setBMSConversionState("CONVERSION_OFF");
    }
    writeBMSData(0x49, registerAddress, data);

    Serial.print("Numerical Command sent: ");
    Serial.print(command);
    Serial.print(" with data 0x");
    Serial.println(data, HEX);
}


// Function to send configuration commands to the BMS currently a list format to show each register.
void sendBMSConfigCommand(const char* command) {
    uint8_t registerAddress;
    uint16_t data;

    // Map commands to register addresses and data values
    if (strcmp(command, "CFG2_FILTERS_CYCLES") == 0) {
        registerAddress = 0x04; // Activates what is been sensed and where the fets are.
        data = 0x19FF;          // this activates all sensors and deactivates crc, ovc and sc_en. Fets left in default
    } else if (strcmp(command, "CSA_GAIN_FACTOR") == 0) {
        registerAddress = 0x05; // this is a gain factor for the Current sense amplifier.
        data = 0x8000;          // ive left it as the default. Needs measuring and setting.
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
        registerAddress = 0x0B; // Opposite to above. Sets the undervoltage threshold for the VB pin. and the number of times its allowed.
        data = 0x0F70;          // Set the times allowed to saturation, set the trip point to  EQ =10.93=16*6.1mV*0111 0000
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
    } else if (strcmp(command, "NVM_2_DL") == 0) {
        registerAddress = 0x20; // THIS COMMAND CAN ONLY BE ISSUED 32 TIMES!!! This is the command to commit the changes to the NVM FROM I2C
        data = 0xAAAA;          // NEVER PUT THIS IN THE MAIN LOOP, SETUP ONLY!
    } else if (strcmp(command, "NVM_2_UL") == 0) {
        registerAddress = 0x20; // This is the command to commit the DATA from the NVM TO I2C, this should be called on startup.
        data = 0x5555;          // This is the command as seen in the datasheets.
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
}

//Function to assign identity and and other unique information to the bms chip. these have no bearing on performance so are ideal to test commands.
void sendBMSIdentityCommand(const char* command, uint16_t data) {
    uint8_t registerAddress;

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