// PWR_MNGMT_FCT.h
// ---------------
// Function declarations for power management on the central control board.
// Includes functions to control the main current path, precharge path, 
// read voltage through the pack sense pin, and connect an external charger.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#include <Arduino.h>


float readPackSense(); // Function to read voltage through the pack sense pin
void setDSG(bool state); // Function to turn on the main current path
void setCHG(bool state); // Function to turn on the current path through a precharge resistor
void precharge(int numCapacitors); // Function to precharge the capacitors