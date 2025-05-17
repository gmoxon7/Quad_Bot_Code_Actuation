// BMS_NumericalCommands.cpp
// -------------------------
// Implementation of functions for sending numerical commands to the BMS (Battery Management System) module.
// Handles register threshold and calibration commands, including value conversion and data packing.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.


#include <Arduino.h>
#include "BMS_NumericalCommands.h"
#include "BMS_CoreCommands.h"





float senseResistor = 0.008f; // 8 mΩ = 0.008 Ω (default, can be changed elsewhere)
const float voltageLimitRangeExt = 0.300f; // 300mV as per datasheet

float Imax = voltageLimitRangeExt / senseResistor;
float Vcur_res = voltageLimitRangeExt / 32767.0f; // 32767 = max positive value for 16-bit signed integer




void sendBMSNumericalCommand(const char* command, const char* arg1, const char* arg2) {

//when passing arguments pass the command, the 8lsb argumennt, and the 8msb argument.

    uint8_t registerAddress=0;
    uint16_t data=0;

    // Helper lambda to convert "max"/"min" or string to value
    // For integer fields
auto parseIntArg = [](const char* arg, int minVal, int maxVal) -> int {
    if (!arg) return minVal;
    if (strcasecmp(arg, "max") == 0) return maxVal;
    if (strcasecmp(arg, "min") == 0) return minVal;
    long val = strtol(arg, nullptr, 10);
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return (int)val;
};

// For float fields
auto parseFloatArg = [](const char* arg, float minVal, float maxVal) -> float {
    if (!arg) return minVal;
    if (strcasecmp(arg, "max") == 0) return maxVal;
    if (strcasecmp(arg, "min") == 0) return minVal;
    float val = atof(arg);
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
};

    // Map commands to register addresses and value ranges
    if (strcmp(command, "CSA_GAIN_FACTOR") == 0) {
        registerAddress = 0x05;
        data = parseIntArg(arg1, 0x0000, 0xFFFF);
    }
    
    
    else if (strcmp(command, "VCELL_OV_TH") == 0) {
        registerAddress = 0x06;
        // Hardcoded min/max for this register
        const float VCELL_OV_TH_MIN = 4.3f;
        const float VCELL_OV_TH_MAX = 5.0f;
        const int NCELL_OV_CNT_TH_MIN = 3;
        const int NCELL_OV_CNT_TH_MAX = 15;
        const float VCELL_RES = 0.00122f; // Example value, check your datasheet

        // Parse voltage argument, supporting "min"/"max"
        float voltage = parseFloatArg(arg1, VCELL_OV_TH_MIN, VCELL_OV_TH_MAX);
        int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
        if (vcell_code < 0) vcell_code = 0;
        if (vcell_code > 0xFF) vcell_code = 0xFF;

        // Parse count argument, supporting "min"/"max"
        int ncell_ovcnt = parseIntArg(arg2, NCELL_OV_CNT_TH_MIN, NCELL_OV_CNT_TH_MAX);

        // Pack the data: [0000][NCELL_OV_CNT_TH(4)][VCELL_OV_TH(8)]
        data = ((ncell_ovcnt & 0xF) << 8) | (vcell_code & 0xFF);
    }
    
    
    else if (strcmp(command, "VCELL_UV_TH") == 0) {
        registerAddress = 0x07;
        // Hardcoded min/max for this register
        const float VCELL_UV_TH_MIN = 2.0f;
        const float VCELL_UV_TH_MAX = 3.0f;
        const int NCELL_UV_CNT_TH_MIN = 3;
        const int NCELL_UV_CNT_TH_MAX = 15;
        const float VCELL_RES = 0.00122f; // Example value, check your datasheet

        // Parse voltage argument, supporting "min"/"max"
        float voltage = parseFloatArg(arg1, VCELL_UV_TH_MIN, VCELL_UV_TH_MAX);
        int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
        if (vcell_code < 0) vcell_code = 0;
        if (vcell_code > 0xFF) vcell_code = 0xFF;

        // Parse count argument, supporting "min"/"max"
        int ncell_uvcnt = parseIntArg(arg2, NCELL_UV_CNT_TH_MIN, NCELL_UV_CNT_TH_MAX);

        // Pack the data: [0000][NCELL_UV_CNT_TH(4)][VCELL_UV_TH(8)]
        data = ((ncell_uvcnt & 0xF) << 8) | (vcell_code & 0xFF);
    }


    else if (strcmp(command, "VCELL_SEVERE_DELTA_THRS") == 0) {
    registerAddress = 0x08;
    // Hardcoded min/max for this register
    const float VCELL_SEVERE_DELTA_TH_MIN = 2.0f;
    const float VCELL_SEVERE_DELTA_TH_MAX = 5.0f;
    const float VCELL_RES = 0.00122f; // Example value, check your datasheet

    // Parse OV delta argument (arg1), supporting "min"/"max"
    float ov_voltage = parseFloatArg(arg1, VCELL_SEVERE_DELTA_TH_MIN, VCELL_SEVERE_DELTA_TH_MAX);
    int ov_code = (int)((ov_voltage / (16 * VCELL_RES)) + 0.5f);
    if (ov_code < 0) ov_code = 0;
    if (ov_code > 0xFF) ov_code = 0xFF;

    // Parse UV delta argument (arg2), supporting "min"/"max"
    float uv_voltage = parseFloatArg(arg2, VCELL_SEVERE_DELTA_TH_MIN, VCELL_SEVERE_DELTA_TH_MAX);
    int uv_code = (int)((uv_voltage / (16 * VCELL_RES)) + 0.5f);
    if (uv_code < 0) uv_code = 0;
    if (uv_code > 0xFF) uv_code = 0xFF;

    // Pack the data: [OV_CODE(8)][UV_CODE(8)]
    data = ((uv_code & 0xFF) << 8) | (ov_code & 0xFF);
    }


    else if (strcmp(command, "VCELL_BAL_UV_DELTA_TH") == 0) {
    registerAddress = 0x09;
    // Hardcoded min/max for this register (adjust as per your datasheet)
    const float VCELL_BAL_UV_DELTA_TH_MIN = 2.0f;
    const float VCELL_BAL_UV_DELTA_TH_MAX = 5.0f;
    const int NCELL_BAL_UV_CNT_TH_MIN = 3;
    const int NCELL_BAL_UV_CNT_TH_MAX = 15;
    const float VCELL_RES = 0.00122f; // Example value, check your datasheet

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VCELL_BAL_UV_DELTA_TH_MIN, VCELL_BAL_UV_DELTA_TH_MAX);
    int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
    if (vcell_code < 0) vcell_code = 0;
    if (vcell_code > 0xFF) vcell_code = 0xFF;

    // Parse count argument, supporting "min"/"max"
    int ncell_cnt = parseIntArg(arg2, NCELL_BAL_UV_CNT_TH_MIN, NCELL_BAL_UV_CNT_TH_MAX);

    // Pack the data: [0000][NCELL_BAL_UV_CNT_TH(4)][VCELL_BAL_UV_DELTA_TH(8)]
    data = ((ncell_cnt & 0xF) << 8) | (vcell_code & 0xFF);
    } 


    else if (strcmp(command, "VB_OV_TH") == 0) {
    registerAddress = 0x0A;
    const float VB_OV_TH_MIN = 0.0f;
    const float VB_OV_TH_MAX = 25.0f;
    const int NCELL_OV_CNT_TH_MIN = 0;
    const int NCELL_OV_CNT_TH_MAX = 15;
    const float VB_RES = 0.0061f; // 6.1mV

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VB_OV_TH_MIN, VB_OV_TH_MAX);
    int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
    if (vb_code < 0) vb_code = 0;
    if (vb_code > 0xFF) vb_code = 0xFF;

    // Parse count argument, supporting "min"/"max"
    int ncell_ovcnt = parseIntArg(arg2, NCELL_OV_CNT_TH_MIN, NCELL_OV_CNT_TH_MAX);

    // Pack the data: [0000][NCELL_OV_CNT_TH(4)][VB_OV_TH(8)]
    data = ((ncell_ovcnt & 0xF) << 8) | (vb_code & 0xFF);
    }


    else if (strcmp(command, "VB_UV_TH") == 0) {
    registerAddress = 0x0B;
    const float VB_RES = 0.0061f; // 6.1mV
    const float VB_UV_TH_MIN = 0.0f;
    const float VB_UV_TH_MAX = 25.0f;
    const int NCELL_UV_CNT_TH_MIN = 0;
    const int NCELL_UV_CNT_TH_MAX = 15;

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VB_UV_TH_MIN, VB_UV_TH_MAX);
    int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
    if (vb_code < 0) vb_code = 0;
    if (vb_code > 0xFF) vb_code = 0xFF;

    // Parse count argument, supporting "min"/"max"
    int ncell_uvcnt = parseIntArg(arg2, NCELL_UV_CNT_TH_MIN, NCELL_UV_CNT_TH_MAX);

    // Pack the data: [0000][NCELL_UV_CNT_TH(4)][VB_UV_TH(8)]
    data = ((ncell_uvcnt & 0xF) << 8) | (vb_code & 0xFF);
    }


    else if (strcmp(command, "VB_SUM_MAX_DIFF_TH") == 0) {
    registerAddress = 0x0C;
    const float VB_SUM_MAX_DIFF_TH_MIN = 0.0f;
    const float VB_SUM_MAX_DIFF_TH_MAX = 25.0f;
    const float VB_RES = 0.0061f; // 6.1mV

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VB_SUM_MAX_DIFF_TH_MIN, VB_SUM_MAX_DIFF_TH_MAX);
    int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
    if (vb_code < 0) vb_code = 0;
    if (vb_code > 0xFF) vb_code = 0xFF;

    // Pack the data: [00000000][VB_SUM_MAX_DIFF_TH(8)]
    data = vb_code & 0xFF;
    }


    else if (strcmp(command, "VNTC_OT_TH") == 0) {
    registerAddress = 0x0D;
    const float VNTC_OT_TH_MIN = 0.2f;
    const float VNTC_OT_TH_MAX = 3.3f;
    const int NNTC_OT_CNT_TH_MIN = 0;
    const int NNTC_OT_CNT_TH_MAX = 15;
    const float VNTC_RES = 0.000806f;

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VNTC_OT_TH_MIN, VNTC_OT_TH_MAX);
    int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
    if (ntc_code < 0) ntc_code = 0;
    if (ntc_code > 0xFFF) ntc_code = 0xFFF;

    // Parse count argument, supporting "min"/"max"
    int nntc_cnt = parseIntArg(arg2, NNTC_OT_CNT_TH_MIN, NNTC_OT_CNT_TH_MAX);

    // Pack the data: [NNTC_OT_CNT_TH(4)][NTC_OT_TH(12)]
    data = ((nntc_cnt & 0xF) << 12) | (ntc_code & 0xFFF);
    }



    else if (strcmp(command, "VNTC_UT_TH") == 0) {
    registerAddress = 0x0E;
    const float VNTC_UT_TH_MIN = 0.2f;
    const float VNTC_UT_TH_MAX = 3.3f;
    const int NNTC_UT_CNT_TH_MIN = 0;
    const int NNTC_UT_CNT_TH_MAX = 15;
    const float VNTC_RES = 0.000806f;

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VNTC_UT_TH_MIN, VNTC_UT_TH_MAX);
    int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
    if (ntc_code < 0) ntc_code = 0;
    if (ntc_code > 0xFFF) ntc_code = 0xFFF;

    // Parse count argument, supporting "min"/"max"
    int nntc_cnt = parseIntArg(arg2, NNTC_UT_CNT_TH_MIN, NNTC_UT_CNT_TH_MAX);

    // Pack the data: [NNTC_UT_CNT_TH(4)][NTC_UT_TH(12)]
    data = ((nntc_cnt & 0xF) << 12) | (ntc_code & 0xFFF);
    }


    else if (strcmp(command, "VNTC_SEVERE_OT_DELTA_TH") == 0) {
    registerAddress = 0x0F;
    const float VNTC_SEVERE_OT_DELTA_TH_MIN = 0.2f;
    const float VNTC_SEVERE_OT_DELTA_TH_MAX = 3.3f;
    const float VNTC_RES = 0.000806f;

    // Parse voltage argument, supporting "min"/"max"
    float voltage = parseFloatArg(arg1, VNTC_SEVERE_OT_DELTA_TH_MIN, VNTC_SEVERE_OT_DELTA_TH_MAX);
    int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
    if (ntc_code < 0) ntc_code = 0;
    if (ntc_code > 0xFFF) ntc_code = 0xFFF;

    // Pack the data: [0000][NTC_SEVERE_OT_DELTA_TH(12)]
    data = ntc_code & 0xFFF;
    }



    else if (strcmp(command, "OVC_THRESHOLDS") == 0) {
    registerAddress = 0x10;
    extern float Imax;

    // Parse and clamp charge current (arg1)
    float chg_current = parseFloatArg(arg1, 0.0f, Imax);
    int chg_code = (int)((chg_current / Imax) * 255.0f + 0.5f);
    if (chg_code < 0) chg_code = 0;
    if (chg_code > 0xFF) chg_code = 0xFF;

    // Parse and clamp discharge current (arg2)
    float dchg_current = parseFloatArg(arg2, 0.0f, Imax);
    int dchg_code = (int)((dchg_current / Imax) * 255.0f + 0.5f);
    if (dchg_code < 0) dchg_code = 0;
    if (dchg_code > 0xFF) dchg_code = 0xFF;

    // Pack the data: [OVC_DCHG_TH(8)][OVC_CHG_TH(8)]
    data = ((dchg_code & 0xFF) << 8) | (chg_code & 0xFF);
    }


    
    else if (strcmp(command, "PERSISTENT_OVC_THRESHOLDS") == 0) {
    registerAddress = 0x11;
    extern float Imax;

    // Parse and clamp current argument
    float current = parseFloatArg(arg1, 0.0f, Imax);
    int code = (int)((current / Imax) * 255.0f + 0.5f);
    if (code < 0) code = 0;
    if (code > 0xFF) code = 0xFF;

    // Pack the data: [00000000][CODE(8)]
    data = code & 0xFF;
}


else if (strcmp(command, "SC_THRESHOLD") == 0) {
    registerAddress = 0x12;
    extern float senseResistor;

    // Helper lambda to convert current to code for SC_TH and SC_PERSIST_TH
    auto currentToCode = [&](float current) -> int {
        // Clamp current to minimum possible (for CODE=0)
        float minVoltage = 0.04914f;
        float maxVoltage = 0.04914f + 0.01404f * 15.0f; // 0.26074V
        float minCurrent = minVoltage / senseResistor;
        float maxCurrent = maxVoltage / senseResistor;

        if (current < minCurrent) current = minCurrent;
        if (current > maxCurrent) current = maxCurrent;

        float voltage = current * senseResistor;
        int code = (int)(((voltage - 0.04914f) / 0.01404f) + 0.5f);
        if (code < 0) code = 0;
        if (code > 15) code = 15;
        return code;
    };

    // Parse and clamp SC_TH (arg1)
    float sc_current = parseFloatArg(arg1, 0.0f, 1000.0f); // 1000A is a safe upper bound
    int sc_code = currentToCode(sc_current);

    // Parse and clamp SC_PERSIST_TH (arg2)
    float sc_persist_current = parseFloatArg(arg2, 0.0f, 1000.0f);
    int sc_persist_code = currentToCode(sc_persist_current);

    // Pack the data: [00000000][SC_PERSIST_TH(4)][SC_TH(4)]
    data = ((sc_persist_code & 0xF) << 4) | (sc_code & 0xF);
}



else {
    Serial.print("Numerical Command not recognized: ");
    Serial.println(command);
    return;
   
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
}
/*
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


*/




