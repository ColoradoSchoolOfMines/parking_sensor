/*
Arduino, HMC5883, magnetometer, XBee code 
 by: Roy Stillwell
 Colorado School of Mines
 created on: 3.30.13
 license: This work is licensed under a Creative Commons Attribution license.
 
 Arduino code example for interfacing with the HMC5883 
 by: Jordan McConnell
 SparkFun Electronics
 created on: 6/30/11
 license: OSHW 1.0, http://freedomdefined.org/OSHW
 
 Analog input 4 I2C SDA
 Analog input 5 I2C SCL
 
 EEPROM code adapted from Tomorrow Lab
 Developer: Ted Ullricis_measuringh <ted@tomorrow-lab.com>
 http://tomorrow-lab.com
 
*/

#include <Wire.h> //I2C Arduino Library
#include <EEPROM.h>
#include "EEPROMAnything.h"


//#include "Statistic.h"

#define NOTE_C6  1047
#define NOTE_E6  1319
#define NOTE_G6  1568
#define MEM_SIZE 512 //EEPROM memory size (remaining 2 bytes reserved for count)

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

//Statistic StatsX;
//Statistic StatsY;
//Statistic StatsZ;

int pointer = 0;
int x,y,z; //triple axis data

void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
//  StatsX.clear(); //clear the stat list
//  StatsY.clear();
//  StatsZ.clear();

  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();


//  while (pointer < 300 ) {  //Initialize the storage arrays

    //Tell the HMC5883 where to begin reading data
    Wire.beginTransmission(address);
    Wire.write(0x03); //select register 3, X MSB register
    Wire.endTransmission();

    //Read data from each axis, 2 registers per axis
    Wire.requestFrom(address, 6);
    if(6<=Wire.available()){
      x = Wire.read()<<8; //X msb
      x |= Wire.read(); //X lsb
      z = Wire.read()<<8; //Z msb
      z |= Wire.read(); //Z lsb
      y = Wire.read()<<8; //Y msb
      y |= Wire.read(); //Y lsb
    }

//    StatsX(pointer) = x;
//    pointer++;
//  }
}

void loop(){

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
  
  //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
  }

//  StatsX(pointer)= x;  //once array is at 300, start re-using data slots
//  pointer++;
//  if ( pointer == 300) {
//    pointer = 0;  //resests pointer for running window  
//  }
  //StatsX.add(y);
  //StatsX.add(z);


  //  Serial.print("  Count: ");
  //  Serial.print(StatsX.count());
  //
  //  Serial.print("  Average: ");
  //  Serial.print(StatsX.average(), 4);
  //
  //  Serial.print("  Std deviation: ");
  //
  //  Serial.print(StatsX.pop_stdev(), 4);
  //  Serial.println();




  //Print out values of each axis
  //Serial.print("x: ");
//  if (x < (StatsX.average() - int(2 * StatsX.pop_stdev())) || x > (StatsX.average() + int(2 * StatsX.pop_stdev()))) 
//  {
//    Serial.println("Car found!!");
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    Serial.print(" ");
    Serial.println(z);
//  }
delay(40);
}






