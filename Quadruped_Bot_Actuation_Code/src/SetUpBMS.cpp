// SetUpBMS.cpp
// ------------
// Applies all configuration and numerical commands to initialize and configure the BMS (Battery Management System).
// This function should be called during setup to ensure the BMS is correctly configured before operation.
// If configuration is stored in NVM (Non-Volatile Memory), this setup can be commented out on startup.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.



#include "Arduino.h"

#include "BMS_CoreCommands.h"
#include "BMS_SetupCommands.h"
#include "BMS_NumericalCommands.h"


// Function to send identity commands to the BMS

void setUpBMS() {

/*
    // Activates what is being sensed and where the FETs are.
    // This activates all sensors and deactivates CRC, OVC, and SC_EN. FETs left in default.
    sendBMSConfigCommand("CFG2_FILTERS_CYCLES");

    // These are masking bits. They prevent the device tripping the FETs or relevant registers if an error is detected.
    // This will want to be implemented in future.
    sendBMSConfigCommand("TO_PRDV_BAL_MSK");

    // Mask the actuation of the fuse. These aren't in use on the board but could be in future.
    sendBMSConfigCommand("TO_FUSE_RST_MSK");

    // Masking the FAULTN pin. This is the most likely register to be changed in future as it can be directly sensed from the Teensy.
    sendBMSConfigCommand("TO_FAULTN_MSK");

    // More masking registers, pertaining to current.
    sendBMSConfigCommand("CURR_MSK");

    
    // These are all diagnostic flags. This command will reset them; can expand on them to be read at a later date.
    sendBMSConfigCommand("DIAG_OV_OT_UT");

    // Registers that denote under voltage events.
    sendBMSConfigCommand("DIAG_UV");

    // Diagnostic flags for current events. For now wipes the flags.
    sendBMSConfigCommand("DIAG_CURR");




/*

	// Set CSA gain factor (current sense amplifier gain)
	// Range: 0x0000 (min) to 0xFFFF (max), default: 0x8000
	sendBMSNumericalCommand("CSA_GAIN_FACTOR", "default", nullptr);

	// Set cell overvoltage threshold and count
	// Voltage: 0V (min) to 5.0V (max), default: 4.3V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VCELL_OV_TH", "max", "max");

	// Set cell undervoltage threshold and count
	// Voltage: 0V (min) to 5.0V (max), default: 2.2V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VCELL_UV_TH", "min", "max");

	// Set cell severe delta thresholds (overvoltage, undervoltage)
	// OV Delta: 0V (min) to 5.0V (max), default: 0.2V
	// UV Delta: 0V (min) to 5.0V (max), default: 0.2V
	sendBMSNumericalCommand("VCELL_SEVERE_DELTA_THRS", "max", "min");

	// Set cell balancing undervoltage delta threshold and count
	// Voltage: 0V (min) to 5.0V (max), default: 0.2V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VCELL_BAL_UV_DELTA_TH", "min", "max");

	// Set battery block overvoltage threshold and count
	// Voltage: 0V (min) to 25.0V (max), default: 23.0V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VB_OV_TH", "max", "max");

	// Set battery block undervoltage threshold and count
	// Voltage: 0V (min) to 25.0V (max), default: 10.93V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VB_UV_TH", "min", "max");

	// Set battery block sum max difference threshold
	// Voltage: 0V (min) to 25.0V (max), default: 2.0V
	sendBMSNumericalCommand("VB_SUM_MAX_DIFF_TH", "max", nullptr);

	// Set NTC (thermistor) overtemperature threshold and count
	// Voltage: 0.2V (min) to 3.3V (max), default: 2.5V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VNTC_OT_TH", "max", "max");

	// Set NTC (thermistor) undertemperature threshold and count
	// Voltage: 0.0V (min) to 3.3V (max), default: 0.5V
	// Count: 1 (min) to 15 (max), default: 15
	sendBMSNumericalCommand("VNTC_UT_TH", "min", "max");

	// Set NTC severe overtemperature delta threshold
	// Voltage: 0.0V (min) to 3.3V (max), default: 0.5V
	sendBMSNumericalCommand("VNTC_SEVERE_OT_DELTA_TH", "max", nullptr);

	// Set overcurrent thresholds (charge, discharge)
	// Current: 0A (min) to Imax (max, calculated), default: Imax
	sendBMSNumericalCommand("OVC_THRESHOLDS", "max", "max");

	// Set persistent overcurrent threshold
	// Current: 0A (min) to Imax (max, calculated), default: Imax
	sendBMSNumericalCommand("PERSISTENT_OVC_THRESHOLDS", "max", nullptr);

	// Set short-circuit threshold and persistence threshold
	// Current: ~6.1A (min, depends on senseResistor) to ~32.5A (max, depends on senseResistor), default: 100A
	sendBMSNumericalCommand("SC_THRESHOLD", "max", "max");
    */


}


