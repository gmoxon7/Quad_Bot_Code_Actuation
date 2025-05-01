//  PWR_MNGMT_FCT.h
//
// This file contains the function declarations for power management functions
// It includes functions to turn on the main current path,
// turn on the current path through a precharge resistor read voltage through the
// pack sense pin and connect an external charger to the battery pack.

#include <Arduino.h>


float readPackSense(); // Function to read voltage through the pack sense pin
void setDSG(bool state); // Function to turn on the main current path
void setCHG(bool state); // Function to turn on the current path through a precharge resistor
void precharge(int numCapacitors); // Function to precharge the capacitors