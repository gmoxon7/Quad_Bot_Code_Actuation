//Functions for the I2C to operate

#include "I2C_MUX.h"
//function to write to a specific I2C address
void I2C_WR(unsigned char ADDRESS, unsigned char I2C_OPERATION) {
  Wire.beginTransmission(ADDRESS);
  Wire.write(I2C_OPERATION);
  Wire.endTransmission();
}
//function to read from an i2c device. 
//this only takes one address so a second address will be needed to read from specific registers.
unsigned char I2C_RD(unsigned char ADDRESS) {
  Wire.beginTransmission(ADDRESS);
  Wire.endTransmission(false);
  Wire.requestFrom(ADDRESS, 1);

  while (!Wire.available()) {
  }

  return Wire.read();
}

