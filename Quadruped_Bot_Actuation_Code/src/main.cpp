#include <Arduino.h>


#include <Wire.h>
//#include <Wire1.h> // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(sc1)

#include <SPI.h> // for the rotary encoder

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}