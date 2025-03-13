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
digitalWrite(NRESET, HIGH);
digitalWrite(SERVO_REG_ENABLE, LOW);

delay(1000); //delay for the control board to initialize

  Wire.begin(); //initialize the i2c bus
  Wire1.begin(); //initialize the second i2c bus
  
  
  
  Serial.begin(9600);

  delay(15);

  while (!Serial)
  delay(10);
  Serial.println("\nI2C Scanner");
 


}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(EN_PIN, HIGH); //turn on the control board interfaces
  digitalWrite(NRESET, LOW); //t




  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}

// put function definitions here:

//This Function is a basic i2c function for controlling the motor driver

