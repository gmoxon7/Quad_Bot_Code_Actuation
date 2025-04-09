#include <Arduino.h>

#include <Wire.h>
//#include <Wire1.h> // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(sc1)

#include <SPI.h> // for the rotary encoder

//Definitions
#define WIRE Wire // for teensy 4.0 this is the i2c library on pins 18(sda) and 19(scl)
//#define WIRE Wire1 // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(scl1)

#define PRE_DSG_EN 2 //precharge discharge enable
#define DSG_EN 3  //discharge enable
#define CHG_EN 4  //charge enable

#define STROBE 5 //strobe signal, This latches multiplexer when low, live update when high
#define CS_1 6 //Chip select pin numbers to the multiplexer. CS1 is LSB, CS4 is MSB
#define CS_2 7
#define CS_3 8
#define CS_4 9
#define INHIBIT 10 //INHIBIT is high all outputs on SPI multiplexer are 0 

#define NFAULT 14 //Signal to the L9961 bms chip recieves information  status of the battery pack, input
#define RDY 15 //RDY interrupt output. will be an input

#define EN_PIN 20 //output that turns on a range of chips on the control board, output
#define NRESET 21 //reset pin to the I2C multiplexer, output
#define SERVO_REG_ENABLE 22 //enable pin for the servo regulator, output

#define PACK_SNS 23 //pack sense input, input

unsigned char ADDRESS = 0x70; //I2C address of the multiplexer write version increment for read version
unsigned char I2C_OPERATION = 0x01; //I2C operation to select channel 0

uint8_t MUX_channel = 0; //SPI multiplexer channel number 0 default
// put function declarations here:

//Functions

void I2C_WR(unsigned char ADDRESS, unsigned char I2C_OPERATION)
{
  Wire.beginTransmission(ADDRESS);
  Wire.write(I2C_OPERATION);
  Wire.endTransmission();
}
//note the lack of data obviously because that is what we are getting in this process.
unsigned char I2C_RD(unsigned char ADDRESS)
{
  Wire.beginTransmission(ADDRESS); //Starts communicating with the device at 
  Wire.endTransmission(false); // creates the repeat start bit needed to read from the register
  //The number of bytes requested. queues the stop bit.
  Wire.requestFrom(ADDRESS,1);

  while(!Wire.available())
  {
  }
  
  
  return Wire.read();
}

void SPI_CS_MUX(uint8_t newChannel)
 {
  //This function selects the channel on the SPI multiplexer
  //The channel is selected by setting the CS pins to the binary value of the channel number
  //The 4 CS pins are passed through a mux to get 16 outputs.
  if (newChannel > 15) {
    Serial.println("Error: Invalid channel requested! Must be between 0 and 15");
  return; // invalid channel number
  }
  MUX_channel = newChannel;
   // Set the multiplexer control pins based on the channel
   digitalWrite(CS_1, MUX_channel & 0x01);       // LSB
   digitalWrite(CS_2, (MUX_channel >> 1) & 0x01);
   digitalWrite(CS_3, (MUX_channel >> 2) & 0x01);
   digitalWrite(CS_4, (MUX_channel >> 3) & 0x01); // MSB

}

void ACTIVATE_MUX(uint8_t SPI_channel){
// Select the desired channel
SPI_CS_MUX(SPI_channel);

// Latch the selected channel
digitalWrite(STROBE, HIGH); //allows registers to output current input logic
delay(10); // Small delay for stability
digitalWrite(STROBE, LOW); // latches the outputs to last input logic, preventing change during transmission
delay(10); // Small delay for stability

// Enable the multiplexer outputs
digitalWrite(INHIBIT, LOW); //outputs latched logic
delayMicroseconds(3); // Small delay for stability

}

void DEACTIVATE_MUX() {
  // Disable all multiplexer outputs
  digitalWrite(INHIBIT, HIGH);
  delayMicroseconds(3); // Small delay for stability
}


uint16_t readEncoderPosition(uint8_t channel) {
  uint16_t position = 0;

  // Activate the multiplexer for the desired channel
  ACTIVATE_MUX(channel);// puts strobe in the correct state latches the outputs to the last input logic
  delayMicroseconds(3); // Small delay to establish cs for amt223cv 
  // Start SPI transaction with SPI Mode 0 and 100kHz clock speed
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));

  // Send dummy bytes and read the 16-bit response
    position = SPI.transfer(0x00) <<8 ; // Send dummy data and receive 8 bits?
    delayMicroseconds(3); // Small delay as prescribed by the amt223cv datasheet
    position |= SPI.transfer(0x00);
    delayMicroseconds(3); // Small delay as prescribed by the amt223cv datasheet

  // End SPI transaction
  SPI.endTransaction();
delayMicroseconds(3); // Small delay as prescribed by the amt223cv datasheet
  // Deactivate the multiplexer
  DEACTIVATE_MUX();

  // Extract the 12-bit position (ignore the first 2 bits and last 2 bits)
  position = (position >> 2) & 0x0FFF; // Mask to get the 12-bit value
  // The position is now in the range of 0 to 4095 (12 bits)

  return position;
}


// put setup code here, to run once:

void setup() {
  // put your setup code here, to run once:

  //defining pin directions
pinMode(PRE_DSG_EN, OUTPUT);
pinMode(DSG_EN, OUTPUT);  
pinMode(CHG_EN, OUTPUT);

pinMode(STROBE, OUTPUT);
pinMode(CS_1, OUTPUT);
pinMode(CS_2, OUTPUT);
pinMode(CS_3, OUTPUT);
pinMode(CS_4, OUTPUT);
pinMode(INHIBIT, OUTPUT);

pinMode(NFAULT, INPUT);
pinMode(RDY, INPUT);

pinMode(EN_PIN, OUTPUT);
pinMode(NRESET, OUTPUT);

pinMode(SERVO_REG_ENABLE, OUTPUT);

pinMode(PACK_SNS, INPUT);

//setting pin  positions for startup
digitalWrite(PRE_DSG_EN, LOW);
digitalWrite(DSG_EN, LOW);
digitalWrite(CHG_EN, LOW);
digitalWrite(STROBE, LOW);
digitalWrite(CS_1, LOW);
digitalWrite(CS_2, LOW);
digitalWrite(CS_3, LOW);
digitalWrite(CS_4, LOW);
digitalWrite(INHIBIT, HIGH);
digitalWrite(EN_PIN, LOW);
digitalWrite(NRESET, LOW);
digitalWrite(SERVO_REG_ENABLE, LOW);

delay(1000); //delay for the control board to initialize

  Serial.begin(9600);
  Wire.begin(); //initialize the i2c bus
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Initialized");
  delay(100);
  SPI.begin(); //initialize the SPI bus
  delay(100);
  Serial.println("\nSPI Initialized");

  digitalWrite(EN_PIN, HIGH);
  delay(10); //delay for the Peripheral ICS to initialize
  digitalWrite(NRESET, LOW);
  delay(10);
  digitalWrite(NRESET, HIGH);
  delay(10);

}




void loop() {
  // put your main code here, to run repeatedly:
//I2C multiplexer channel select and read code.
/*
  I2C_WR(0x70,0x00); //Select the I2C multiplexer

  delay(10);

  Serial.print("\nI2C Multiplexer Channels: ");
  Serial.print(I2C_RD(0x70),HEX);
*/

  // ACTIVATE_MUX(5); // Select and activate channel 5
  // Serial.println("Multiplexer activated on channel 5");
  // delay(3000); // Wait for 3 second

  // // Deactivate the multiplexer
  // DEACTIVATE_MUX(); // Disable all multiplexer outputs
  // Serial.println("Multiplexer deactivated");
  // delay(3000); // Wait for 3 second

  // // Activate the multiplexer on channel 10
  // ACTIVATE_MUX(10); // Select and activate channel 10
  // Serial.println("Multiplexer activated on channel 10");
  // delay(3000); // Wait for 3 second

  // // Deactivate the multiplexer again
  // DEACTIVATE_MUX();
  // Serial.println("Multiplexer deactivated");
  // delay(3000); // Wait for 3 second

//testing the SPI encoder 
 // Read position from encoder on channel 0
 uint16_t position = readEncoderPosition(0);
 Serial.print("Encoder position on channel 0: ");
 Serial.println(position);

 delay(1000); // Wait for 1 second

 // Read position from encoder on channel 3
 position = readEncoderPosition(3);
 Serial.print("Encoder position on channel 3: ");
 Serial.println(position);

 delay(1000); // Wait for 1 second



}

