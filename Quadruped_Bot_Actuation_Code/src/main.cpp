#include <Arduino.h>

#include <Wire.h>
//#include <Wire1.h> // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(sc1)

#include <SPI.h> // for the rotary encoder

//Definitions
#define WIRE Wire // for teensy 4.0 this is the i2c library on pins 18(sda) and 19(scl)
//#define WIRE Wire1 // for teensy 4.0 this is the i2c library on pins 17(sda1) and 16(scl1)

// put function declarations here:

// Function declarations
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
  Serial.println("\nI2C Scanner");
  delay(100);


}