/*
  Change the local altitude reference
  BME280 Arduino and Teensy example
  Marshall Taylor @ SparkFun Electronics
  v1.1 by Nathan Seidle @ SparkFun Electronics
  May 20, 2015
  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14348 - Qwiic Combo Board
  https://www.sparkfun.com/products/13676 - BME280 Breakout Board

  This sketch configures a BME280 to measure height changes, with a button
  to zero the local relative altitude.

  To use, run the sketch then press a button on the keyboard. The data on the
  serial monitor will show zero.  Then, move the sensor to a new altitude and watch
  how the output changes!

  Note: For most accurate results keep the sensor orientation and temperature the
  same between measurements.

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
  Please review the LICENSE.md file included with this example. If you have any questions
  or concerns with licensing, please contact techsupport@sparkfun.com.
  Distributed as-is; no warranty is given.
*/

#include <Wire.h>

#include "SparkFunBME280.h"
BME280 mySensor;

#include "CircularBuffer.h"

//This is a fixed-size buffer used for averaging data, a software LP filter
//Depending on the settings, the response can be quite slow.
CircularBuffer myBuffer(50);

float localAltitude = 0;

void setup()
{
  Serial.begin(115200);
  while(!Serial); //Wait for user to get terminal open
  Serial.println("Output a local changing altitude");
  Serial.println("Press any key to zero local altitude");

  Wire.begin();

  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while (1); //Freeze
  }

  //filter can be off or number of FIR coefficients to use:
  //  0, filter off
  //  1, coefficients = 2
  //  2, coefficients = 4
  //  3, coefficients = 8
  //  4, coefficients = 16
  mySensor.setFilter(4); //Lots of HW filter

  //tempOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.setTempOverSample(5); //Set oversample to 16

  //pressOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.setPressureOverSample(5); //Set oversample to 16

  //humidOverSample can be:
  //  0, skipped
  //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
  mySensor.setHumidityOverSample(0); //Turn off humidity sensing

  //Assume the current reading is where we should start
  localAltitude = mySensor.readFloatAltitudeFeet(); //Set reference altitude

  for(int x = 0 ; x < 50 ; x++)
    myBuffer.pushElement(0); //Clear buffer
}

void loop()
{
  //Get the local temperature!  Do this for calibration
  mySensor.readTempC();

  //Check to see if user pressed a button
  if (Serial.available() == true)
  {
    localAltitude = mySensor.readFloatAltitudeFeet(); //Set reference altitude
    
    for(int x = 0 ; x < 50 ; x++)
      myBuffer.pushElement(0); //Clear buffer

    Serial.read(); //Remove keypress from buffer
    Serial.println("Local altitude reset");
  }

  float reading = mySensor.readFloatAltitudeFeet();
  myBuffer.pushElement(reading - localAltitude);

  float tempAlt = myBuffer.averageLast(15); //Do an average of the latest samples

  Serial.print("Last sample: ");
  Serial.print(reading, 2);
  Serial.print(" ft");

  Serial.print(" Last 15 samples averaged: ");
  Serial.print(tempAlt);
  Serial.print(" ft");
  Serial.println();
  delay(200);
}
