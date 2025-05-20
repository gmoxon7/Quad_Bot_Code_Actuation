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

void SetUpBMS() {

    //RWBMSNVM("NVM_2_UL"); // This is the command to commit the DATA from the NVM TO I2C, this should be called on startup.	

    // Configure main filter and conversion cycles (affects ADC filtering and speed)
    sendBMSConfigCommand("CFG1_FILTERS_CYCLES", "default");

    // Configure which cells are enabled for balancing
    sendBMSConfigCommand("TO_PRDV_BAL_MSK", "default");

    // Configure which fuses are enabled for reset (controls fuse reset mask)
    sendBMSConfigCommand("TO_FUSE_RST_MSK", "default");

    // Configure which faults are enabled for nFAULT pin signaling
    sendBMSConfigCommand("TO_FAULTN_MSK", "default");

    // Configure current measurement mask (enables/disables current sensing)
    sendBMSConfigCommand("CURR_MSK", "default");

    // Configure which overvoltage, overtemperature, and undertemperature diagnostics are enabled
    sendBMSConfigCommand("DIAG_OV_OT_UT", "default");

    // Configure which undervoltage diagnostics are enabled
    sendBMSConfigCommand("DIAG_UV", "default");

    // Configure which current diagnostics are enabled
    sendBMSConfigCommand("DIAG_CURR", "default");

    // Example: Send identity commands (replace 0x1234 with your actual data)
    sendBMSIdentityCommand("MANUFACTURE_NAME_MSB", 0x1234);
    sendBMSIdentityCommand("MANUFACTURE_NAME_LSB", 0x5678);
    sendBMSIdentityCommand("MANUFACTURING_DATE", 0x2025);
    sendBMSIdentityCommand("FIRST_USAGE_DATE", 0x2025);
    sendBMSIdentityCommand("SERIAL_NUMBER_MSB", 0xABCD);
    sendBMSIdentityCommand("SERIAL_NUMBER_LSB", 0xEF01);
    sendBMSIdentityCommand("DEVICE_NAME_MSB", 0x1357);
    sendBMSIdentityCommand("DEVICE_NAME_LSB", 0x2468);






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
    


}


