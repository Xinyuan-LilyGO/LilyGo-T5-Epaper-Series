/*
  Read all the regigsters of the BME280
  BME280 Arduino and Teensy example
  Marshall Taylor @ SparkFun Electronics
  May 20, 2015

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14348 - Qwiic Combo Board
  https://www.sparkfun.com/products/13676 - BME280 Breakout Board

  This example outputs all the functional registers from the BME280. This can be handy
  for troubleshooting.

  This sketch configures the BME280 to read all measurements.  The sketch also
  displays the BME280's physical memory and what the driver perceives the
  calibration words to be.
*/

#include <Wire.h>

#include "SparkFunBME280.h"
BME280 mySensor;

void setup()
{
  Serial.begin(115200);
  while(!Serial); //Needed for printing correctly when using a Teensy
  Serial.println("Reading all registers from BME280");

  Wire.begin();

  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while (1); //Freeze
  }

  Serial.print("ID(0xD0): ");
  printyPrintHex(mySensor.readRegister(BME280_CHIP_ID_REG));

  Serial.print("Reset register(0xE0): ");
  printyPrintHex(mySensor.readRegister(BME280_RST_REG));

  Serial.print("ctrl_meas(0xF4): ");
  printyPrintHex(mySensor.readRegister(BME280_CTRL_MEAS_REG));

  Serial.print("ctrl_hum(0xF2): ");
  printyPrintHex(mySensor.readRegister(BME280_CTRL_HUMIDITY_REG));

  Serial.println();

  Serial.println("Displaying all regs:");
  byte memCounter = 0x80;
  for (byte row = 8 ; row < 16 ; row++)
  {
    Serial.print("0x");
    Serial.print(row, HEX);
    Serial.print("0:");

    for (byte column = 0 ; column < 16 ; column++)
    {
      byte tempReadData = mySensor.readRegister(memCounter);

      if(tempReadData < 0x10) Serial.print("0");
      Serial.print(tempReadData, HEX);
      Serial.print(" ");

      memCounter++;
    }
    Serial.println();
  }

  Serial.println();

  Serial.println("Displaying concatenated calibration words:");
  Serial.print("dig_T1, uint16: ");
  Serial.println(mySensor.calibration.dig_T1);
  Serial.print("dig_T2, int16: ");
  Serial.println(mySensor.calibration.dig_T2);
  Serial.print("dig_T3, int16: ");
  Serial.println(mySensor.calibration.dig_T3);

  Serial.print("dig_P1, uint16: ");
  Serial.println(mySensor.calibration.dig_P1);
  Serial.print("dig_P2, int16: ");
  Serial.println(mySensor.calibration.dig_P2);
  Serial.print("dig_P3, int16: ");
  Serial.println(mySensor.calibration.dig_P3);
  Serial.print("dig_P4, int16: ");
  Serial.println(mySensor.calibration.dig_P4);
  Serial.print("dig_P5, int16: ");
  Serial.println(mySensor.calibration.dig_P5);
  Serial.print("dig_P6, int16: ");
  Serial.println(mySensor.calibration.dig_P6);
  Serial.print("dig_P7, int16: ");
  Serial.println(mySensor.calibration.dig_P7);
  Serial.print("dig_P8, int16: ");
  Serial.println(mySensor.calibration.dig_P8);
  Serial.print("dig_P9, int16: ");
  Serial.println(mySensor.calibration.dig_P9);

  Serial.print("dig_H1, uint8: ");
  Serial.println(mySensor.calibration.dig_H1);
  Serial.print("dig_H2, int16: ");
  Serial.println(mySensor.calibration.dig_H2);
  Serial.print("dig_H3, uint8: ");
  Serial.println(mySensor.calibration.dig_H3);
  Serial.print("dig_H4, int16: ");
  Serial.println(mySensor.calibration.dig_H4);
  Serial.print("dig_H5, int16: ");
  Serial.println(mySensor.calibration.dig_H5);
  Serial.print("dig_H6, int8: ");
  Serial.println(mySensor.calibration.dig_H6);

  Serial.println();
}

void loop()
{
  //Each loop, take a reading.
  Serial.print("Humidity: ");
  Serial.print(mySensor.readFloatHumidity(), 0);

  Serial.print(" Pressure: ");
  Serial.print(mySensor.readFloatPressure(), 0);

  Serial.print(" Alt: ");
  //Serial.print(mySensor.readFloatAltitudeMeters(), 1);
  Serial.print(mySensor.readFloatAltitudeFeet(), 1);

  Serial.print(" Temp: ");
  //Serial.print(mySensor.readTempC(), 2);
  Serial.print(mySensor.readTempF(), 2);

  Serial.println();

  delay(1000);
}

//Given a value, print it in HEX with leading 0x and any leading 0s
void printyPrintHex(byte value)
{
  Serial.print("0x");
  if (value < 0x10) Serial.print("0");
  Serial.println(value, HEX);
}

