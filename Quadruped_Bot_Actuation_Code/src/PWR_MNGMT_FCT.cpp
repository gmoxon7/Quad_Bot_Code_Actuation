// PWR_MNGMT_FCT.CPP

// This file contains the function definitions for power management functions
// It includes functions to turn on the main current path, turn on the current path through a precharge resistor, read voltage through the pack sense pin, and connect an external charger to the battery pack.
// It also includes the necessary header files for pin assignments.

#include "PinAssignments.h"
#include "PWR_MNGMT_FCT.h" // Include the header file for power management functions

float readPackSense() {
    int rawValue = analogRead(PACK_SNS);

    // Convert the raw ADC value to voltage at the pin
    float voltageAtPin = (rawValue / 1023.0) * 3.3;

    // Account for the voltage divider (adjust the ratio based on your circuit)
    const float dividerRatio = 20.15/1.683; // value calibration needed in all new boards. derived from experimentation.
    float actualVoltage = voltageAtPin * dividerRatio;

    return actualVoltage;


}

//function to set the discharge diodes in position to allow current to flow from the battery pack to the load
void setDSG(bool state) {
    digitalWrite(DSG_EN, state ? HIGH : LOW); // Replace DSG_PIN with the actual pin name
}

//function to allow a charger to pass current to the battery pack
// Sets the CHG pin to HIGH or LOW
void setCHG(bool state) {
    digitalWrite(CHG_EN, state ? HIGH : LOW); // Replace CHG_PIN with the actual pin name
}


void precharge(int numCapacitors) {
    // Calculate the precharge duration based on the number of capacitors
    const int baseTimeMs = 100; // Base time in milliseconds for one capacitor
    int prechargeDuration = (numCapacitors * baseTimeMs)+100;

    // Activate the precharge resistor
    digitalWrite(PRE_DSG_EN, HIGH);
    
    // Wait for the precharge duration
    delay(prechargeDuration);

    

    // Check the pack voltage
    float packVoltage = readPackSense();
   

    // Ensure the pack voltage is within an acceptable range before enabling DSG
    const float minVoltageThreshold = 10.0; // Example threshold (adjust as needed)
    if (packVoltage >= minVoltageThreshold) {
        // Enable the discharge FETs
        digitalWrite(DSG_EN, HIGH);
        delay(100); // Wait for .1 second to allow the FETs to stabilize
        // Deactivate the precharge resistor
        digitalWrite(PRE_DSG_EN, LOW);

    } else {
        // Log a warning if the voltage is too low
        digitalWrite(PRE_DSG_EN, LOW);
        Serial.println("\n Precharge resistor deactivated.");
        Serial.println("\n Warning: Pack voltage too low. Discharge FETs not enabled.");
    }
}