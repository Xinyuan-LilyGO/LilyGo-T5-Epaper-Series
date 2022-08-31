/*
  Communicate with BME280s using Software I2C
  Nathan Seidle @ SparkFun Electronics
  March 23, 2015

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14348 - Qwiic Combo Board
  https://www.sparkfun.com/products/13676 - BME280 Breakout Board

  This example shows how to connect to sensors using the SoftwareWire library.

  This is an advanced example. You'll need to edit the SparkFunBME280.h file
  and uncomment the #include <SoftwareWire.h> line to enable Software I2C.

  Since we're using software I2C we can use almost any pins. For this example the 
  hardware connections:
  BME280 -> Arduino
  GND -> GND
  3.3 -> 3.3
  SDA -> 6
  SCL -> 7
*/

#include <SoftwareWire.h> //SoftwareWire by Testato. Installed from library manager.

//We use pins 6 and 7 in this example but others can be used
SoftwareWire myWire(6, 7); //SDA, SCL

#include "SparkFunBME280.h"
BME280 mySensor;

void setup()
{
  Serial.begin(115200);
  Serial.println("Example showing alternate I2C addresses");

  myWire.begin();
  if(mySensor.beginI2C(myWire) == false) Serial.println("Sensor A connect failed");
}

void loop()
{
  Serial.print("Humidity: ");
  Serial.print(mySensor.readFloatHumidity(), 0);

  Serial.print(" Pressure: ");
  Serial.print(mySensor.readFloatPressure(), 0);

  Serial.print(" Temp: ");
  //Serial.print(mySensor.readTempC(), 2);
  Serial.print(mySensor.readTempF(), 2);

  Serial.println();

  delay(50);
}

