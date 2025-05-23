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
#include <limits.h>
#include "BMS_NumericalCommands.h"
#include "BMS_CoreCommands.h"





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
        if (strcasecmp(arg, "default") == 0) return INT_MIN; // Sentinel for default
        long val = strtol(arg, nullptr, 10);
        if (val < minVal) return minVal;
        if (val > maxVal) return maxVal;
        return (int)val;
    };
    //for float fields
    auto parseFloatArg = [](const char* arg, float minVal, float maxVal) -> float {
        if (!arg) return minVal;
        if (strcasecmp(arg, "max") == 0) return maxVal;
        if (strcasecmp(arg, "min") == 0) return minVal;
        if (strcasecmp(arg, "default") == 0) return NAN; // Sentinel for default
        float val = atof(arg);
        if (val < minVal) return minVal;
        if (val > maxVal) return maxVal;
    return val;
    };

    if (strcmp(command, "CSA_GAIN_FACTOR") == 0) {
        registerAddress = 0x05;
        int defaultVal = 0x8000; // Default gain factor as seen in the L9961 datasheet
        int val = parseIntArg(arg1, 0x0000, 0xFFFF);
        if (val == INT_MIN) val = defaultVal;
        data = val;
    }
    
    
    else if (strcmp(command, "VCELL_OV_TH") == 0) {
        registerAddress = 0x06;
        // Hardcoded min/max for this register
        const float VCELL_OV_TH_MIN = 0;
        const float VCELL_OV_TH_MAX = 5.0f;
        const int NCELL_OV_CNT_TH_MIN = 1;
        const int NCELL_OV_CNT_TH_MAX = 15;
        const float VCELL_OV_TH_DEFAULT = 4.3f;   // <-- Set your default voltage here
        const int NCELL_OV_CNT_TH_DEFAULT = 15;    // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VCELL_OV_TH_MIN, VCELL_OV_TH_MAX);
        if (isnan(voltage)) voltage = VCELL_OV_TH_DEFAULT;

        int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
        if (vcell_code < 0) vcell_code = 0;
        if (vcell_code > 0xFF) vcell_code = 0xFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int ncell_ovcnt = parseIntArg(arg2, NCELL_OV_CNT_TH_MIN, NCELL_OV_CNT_TH_MAX);
        if (ncell_ovcnt == INT_MIN) ncell_ovcnt = NCELL_OV_CNT_TH_DEFAULT;

        // Pack the data: [0000][NCELL_OV_CNT_TH(4)][VCELL_OV_TH(8)]
        data = ((ncell_ovcnt & 0xF) << 8) | (vcell_code & 0xFF);
    }
    
    
    else if (strcmp(command, "VCELL_UV_TH") == 0) {
        registerAddress = 0x07;
        // Hardcoded min/max for this register
        const float VCELL_UV_TH_MIN = 0;
        const float VCELL_UV_TH_MAX = 5.0f;
        const int NCELL_UV_CNT_TH_MIN = 1;
        const int NCELL_UV_CNT_TH_MAX = 15;
        const float VCELL_UV_TH_DEFAULT = 2.2f;   // <-- Set your default voltage here
        const int NCELL_UV_CNT_TH_DEFAULT = 15;   // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VCELL_UV_TH_MIN, VCELL_UV_TH_MAX);
        if (isnan(voltage)) voltage = VCELL_UV_TH_DEFAULT;

        int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
        if (vcell_code < 0) vcell_code = 0;
        if (vcell_code > 0xFF) vcell_code = 0xFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int ncell_uvcnt = parseIntArg(arg2, NCELL_UV_CNT_TH_MIN, NCELL_UV_CNT_TH_MAX);
        if (ncell_uvcnt == INT_MIN) ncell_uvcnt = NCELL_UV_CNT_TH_DEFAULT;

        // Pack the data: [0000][NCELL_UV_CNT_TH(4)][VCELL_UV_TH(8)]
        data = ((ncell_uvcnt & 0xF) << 8) | (vcell_code & 0xFF);
    }


    else if (strcmp(command, "VCELL_SEVERE_DELTA_THRS") == 0) {
        registerAddress = 0x08;
        // Hardcoded min/max for this register
        const float VCELL_SEVERE_DELTA_TH_MIN = 0;
        const float VCELL_SEVERE_DELTA_TH_MAX = 5.0f;
        const float VCELL_SEVERE_DELTA_TH_OV_DEFAULT = 0.2f; // <-- Set your OV default here
        const float VCELL_SEVERE_DELTA_TH_UV_DEFAULT = 0.2f; // <-- Set your UV default here

        // Parse OV delta argument (arg1), supporting "min"/"max"/"default"
        float ov_voltage = parseFloatArg(arg1, VCELL_SEVERE_DELTA_TH_MIN, VCELL_SEVERE_DELTA_TH_MAX);
        if (isnan(ov_voltage)) ov_voltage = VCELL_SEVERE_DELTA_TH_OV_DEFAULT;
        int ov_code = (int)((ov_voltage / (16 * VCELL_RES)) + 0.5f);
        if (ov_code < 0) ov_code = 0;
        if (ov_code > 0xFF) ov_code = 0xFF;

        // Parse UV delta argument (arg2), supporting "min"/"max"/"default"
        float uv_voltage = parseFloatArg(arg2, VCELL_SEVERE_DELTA_TH_MIN, VCELL_SEVERE_DELTA_TH_MAX);
        if (isnan(uv_voltage)) uv_voltage = VCELL_SEVERE_DELTA_TH_UV_DEFAULT;
        int uv_code = (int)((uv_voltage / (16 * VCELL_RES)) + 0.5f);
        if (uv_code < 0) uv_code = 0;
        if (uv_code > 0xFF) uv_code = 0xFF;

        // Pack the data: [OV_CODE(8)][UV_CODE(8)]
        data = ((uv_code & 0xFF) << 8) | (ov_code & 0xFF);
    }


    else if (strcmp(command, "VCELL_BAL_UV_DELTA_TH") == 0) {
        registerAddress = 0x09;
        // Hardcoded min/max for this register (adjust as per your datasheet)
        const float VCELL_BAL_UV_DELTA_TH_MIN = 0;
        const float VCELL_BAL_UV_DELTA_TH_MAX = 5.0f;
        const int NCELL_BAL_UV_CNT_TH_MIN = 1;
        const int NCELL_BAL_UV_CNT_TH_MAX = 15;
        const float VCELL_BAL_UV_DELTA_TH_DEFAULT = 0.2f; // <-- Set your default voltage here
        const int NCELL_BAL_UV_CNT_TH_DEFAULT = 15;       // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VCELL_BAL_UV_DELTA_TH_MIN, VCELL_BAL_UV_DELTA_TH_MAX);
        if (isnan(voltage)) voltage = VCELL_BAL_UV_DELTA_TH_DEFAULT;
        int vcell_code = (int)((voltage / (16 * VCELL_RES)) + 0.5f);
        if (vcell_code < 0) vcell_code = 0;
        if (vcell_code > 0xFF) vcell_code = 0xFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int ncell_cnt = parseIntArg(arg2, NCELL_BAL_UV_CNT_TH_MIN, NCELL_BAL_UV_CNT_TH_MAX);
        if (ncell_cnt == INT_MIN) ncell_cnt = NCELL_BAL_UV_CNT_TH_DEFAULT;

        // Pack the data: [0000][NCELL_BAL_UV_CNT_TH(4)][VCELL_BAL_UV_DELTA_TH(8)]
        data = ((ncell_cnt & 0xF) << 8) | (vcell_code & 0xFF);
    }


    else if (strcmp(command, "VB_OV_TH") == 0) {
        registerAddress = 0x0A;
        const float VB_OV_TH_MIN = 0.0f;
        const float VB_OV_TH_MAX = 25.0f;
        const int NCELL_OV_CNT_TH_MIN = 1;
        const int NCELL_OV_CNT_TH_MAX = 15;
        const float VB_OV_TH_DEFAULT = 23.0f;   // <-- Set your default voltage here
        const int NCELL_OV_CNT_TH_DEFAULT = 15; // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VB_OV_TH_MIN, VB_OV_TH_MAX);
        if (isnan(voltage)) voltage = VB_OV_TH_DEFAULT;
        int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
        if (vb_code < 0) vb_code = 0;
        if (vb_code > 0xFF) vb_code = 0xFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int ncell_ovcnt = parseIntArg(arg2, NCELL_OV_CNT_TH_MIN, NCELL_OV_CNT_TH_MAX);
        if (ncell_ovcnt == INT_MIN) ncell_ovcnt = NCELL_OV_CNT_TH_DEFAULT;

        // Pack the data: [0000][NCELL_OV_CNT_TH(4)][VB_OV_TH(8)]
        data = ((ncell_ovcnt & 0xF) << 8) | (vb_code & 0xFF);
    }

    else if (strcmp(command, "VB_UV_TH") == 0) {
        registerAddress = 0x0B;
        // Hardcoded min/max for this register
        const float VB_UV_TH_MIN = 0.0f;
        const float VB_UV_TH_MAX = 25.0f;
        const int NCELL_UV_CNT_TH_MIN = 1;
        const int NCELL_UV_CNT_TH_MAX = 15;
        const float VB_UV_TH_DEFAULT = 10.93f;   // <-- Set your default voltage here
        const int NCELL_UV_CNT_TH_DEFAULT = 15;  // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VB_UV_TH_MIN, VB_UV_TH_MAX);
        if (isnan(voltage)) voltage = VB_UV_TH_DEFAULT;
        int vb_code = (int)((voltage / (16 * VB_RES)) + 0.5f);
        if (vb_code < 0) vb_code = 0;
        if (vb_code > 0xFF) vb_code = 0xFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int ncell_uvcnt = parseIntArg(arg2, NCELL_UV_CNT_TH_MIN, NCELL_UV_CNT_TH_MAX);
        if (ncell_uvcnt == INT_MIN) ncell_uvcnt = NCELL_UV_CNT_TH_DEFAULT;

        // Pack the data: [0000][NCELL_UV_CNT_TH(4)][VB_UV_TH(8)]
        data = ((ncell_uvcnt & 0xF) << 8) | (vb_code & 0xFF);
    }


    else if (strcmp(command, "VB_SUM_MAX_DIFF_TH") == 0) {
        registerAddress = 0x0C;
        const float VB_SUM_MAX_DIFF_TH_MIN = 0.0f;
        const float VB_SUM_MAX_DIFF_TH_MAX = 25.0f;
        const float VB_SUM_MAX_DIFF_TH_DEFAULT = 2.0f; // <-- Set your default voltage here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VB_SUM_MAX_DIFF_TH_MIN, VB_SUM_MAX_DIFF_TH_MAX);
        if (isnan(voltage)) voltage = VB_SUM_MAX_DIFF_TH_DEFAULT;
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
        const int NNTC_OT_CNT_TH_MIN = 1;
        const int NNTC_OT_CNT_TH_MAX = 15;
        const float VNTC_OT_TH_DEFAULT = 2.5f;      // <-- Set your default voltage here
        const int NNTC_OT_CNT_TH_DEFAULT = 15;      // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VNTC_OT_TH_MIN, VNTC_OT_TH_MAX);
        if (isnan(voltage)) voltage = VNTC_OT_TH_DEFAULT;
        int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
        if (ntc_code < 0) ntc_code = 0;
        if (ntc_code > 0xFFF) ntc_code = 0xFFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int nntc_cnt = parseIntArg(arg2, NNTC_OT_CNT_TH_MIN, NNTC_OT_CNT_TH_MAX);
        if (nntc_cnt == INT_MIN) nntc_cnt = NNTC_OT_CNT_TH_DEFAULT;

        // Pack the data: [NNTC_OT_CNT_TH(4)][NTC_OT_TH(12)]
        data = ((nntc_cnt & 0xF) << 12) | (ntc_code & 0xFFF);
    }

    else if (strcmp(command, "VNTC_UT_TH") == 0) {
        registerAddress = 0x0E;
        const float VNTC_UT_TH_MIN = 0.0f;
        const float VNTC_UT_TH_MAX = 3.3f;
        const int NNTC_UT_CNT_TH_MIN = 1;
        const int NNTC_UT_CNT_TH_MAX = 15;
        const float VNTC_UT_TH_DEFAULT = 0.5f;      // <-- Set your default voltage here
        const int NNTC_UT_CNT_TH_DEFAULT = 15;      // <-- Set your default count here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VNTC_UT_TH_MIN, VNTC_UT_TH_MAX);
        if (isnan(voltage)) voltage = VNTC_UT_TH_DEFAULT;
        int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
        if (ntc_code < 0) ntc_code = 0;
        if (ntc_code > 0xFFF) ntc_code = 0xFFF;

        // Parse count argument, supporting "min"/"max"/"default"
        int nntc_cnt = parseIntArg(arg2, NNTC_UT_CNT_TH_MIN, NNTC_UT_CNT_TH_MAX);
        if (nntc_cnt == INT_MIN) nntc_cnt = NNTC_UT_CNT_TH_DEFAULT;

        // Pack the data: [NNTC_UT_CNT_TH(4)][NTC_UT_TH(12)]
        data = ((nntc_cnt & 0xF) << 12) | (ntc_code & 0xFFF);
    }

    else if (strcmp(command, "VNTC_SEVERE_OT_DELTA_TH") == 0) {
        registerAddress = 0x0F;
        const float VNTC_SEVERE_OT_DELTA_TH_MIN = 0.0f;
        const float VNTC_SEVERE_OT_DELTA_TH_MAX = 3.3f;
        const float VNTC_SEVERE_OT_DELTA_TH_DEFAULT = 0.5f; // <-- Set your default voltage here

        // Parse voltage argument, supporting "min"/"max"/"default"
        float voltage = parseFloatArg(arg1, VNTC_SEVERE_OT_DELTA_TH_MIN, VNTC_SEVERE_OT_DELTA_TH_MAX);
        if (isnan(voltage)) voltage = VNTC_SEVERE_OT_DELTA_TH_DEFAULT;
        int ntc_code = (int)((voltage / VNTC_RES) + 0.5f);
        if (ntc_code < 0) ntc_code = 0;
        if (ntc_code > 0xFFF) ntc_code = 0xFFF;

        // Pack the data: [0000][NTC_SEVERE_OT_DELTA_TH(12)]
        data = ntc_code & 0xFFF;
    }



    else if (strcmp(command, "OVC_THRESHOLDS") == 0) {
        registerAddress = 0x10;
        extern float Imax;
        const float OVC_THRESHOLDS_DEFAULT_CHG = Imax; // <-- Set your default charge current here
        const float OVC_THRESHOLDS_DEFAULT_DCHG = Imax; // <-- Set your default discharge current here

        // Parse and clamp charge current (arg1)
        float chg_current = parseFloatArg(arg1, 0.0f, Imax);
        if (isnan(chg_current)) chg_current = OVC_THRESHOLDS_DEFAULT_CHG;
        int chg_code = (int)((chg_current / Imax) * 255.0f + 0.5f);
        if (chg_code < 0) chg_code = 0;
        if (chg_code > 0xFF) chg_code = 0xFF;

        // Parse and clamp discharge current (arg2)
        float dchg_current = parseFloatArg(arg2, 0.0f, Imax);
        if (isnan(dchg_current)) dchg_current = OVC_THRESHOLDS_DEFAULT_DCHG;
        int dchg_code = (int)((dchg_current / Imax) * 255.0f + 0.5f);
        if (dchg_code < 0) dchg_code = 0;
        if (dchg_code > 0xFF) dchg_code = 0xFF;

        // Pack the data: [OVC_DCHG_TH(8)][OVC_CHG_TH(8)]
        data = ((dchg_code & 0xFF) << 8) | (chg_code & 0xFF);
    }

    else if (strcmp(command, "PERSISTENT_OVC_THRESHOLDS") == 0) {
        registerAddress = 0x11;
        extern float Imax;
        const float PERSISTENT_OVC_THRESHOLDS_DEFAULT = Imax; // <-- Set your default current here

        // Parse and clamp current argument
        float current = parseFloatArg(arg1, 0.0f, Imax);
        if (isnan(current)) current = PERSISTENT_OVC_THRESHOLDS_DEFAULT;
        int code = (int)((current / Imax) * 255.0f + 0.5f);
        if (code < 0) code = 0;
        if (code > 0xFF) code = 0xFF;

        // Pack the data: [00000000][CODE(8)]
        data = code & 0xFF;
    }

    else if (strcmp(command, "SC_THRESHOLD") == 0) {
        registerAddress = 0x12;
        extern float senseResistor;
        const float SC_THRESHOLD_DEFAULT = 100.0f; // <-- Set your default SC_TH current here
        const float SC_PERSIST_THRESHOLD_DEFAULT = 100.0f; // <-- Set your default SC_PERSIST_TH current here

        // Helper lambda to convert current to code for SC_TH and SC_PERSIST_TH
        auto currentToCode = [&](float current) -> int {
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
        float sc_current = parseFloatArg(arg1, 0.0f, 1000.0f);
        if (isnan(sc_current)) sc_current = SC_THRESHOLD_DEFAULT;
        int sc_code = currentToCode(sc_current);

        // Parse and clamp SC_PERSIST_TH (arg2)
        float sc_persist_current = parseFloatArg(arg2, 0.0f, 1000.0f);
        if (isnan(sc_persist_current)) sc_persist_current = SC_PERSIST_THRESHOLD_DEFAULT;
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





