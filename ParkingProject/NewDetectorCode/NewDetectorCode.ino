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
#include <Arduino.h>
#include "EEPROMAnything.h"


#define NOTE_C6  1047
#define NOTE_E6  1319
#define NOTE_G6  1568
#define MEM_SIZE 512 //EEPROM memory size (remaining 2 bytes reserved for count)

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

const int CALIBRATION_COUNT = 40;
const double ALPHA = 0.005;
const int THRESHOLD = 8;
const int TIMEOUT = 20;

int pointer = 0;
int x,y,z; //triple axis data

int count = 0;
int sum = 0;
int directionSum=0;
double average=0;
double directionAverage=0;
int detectorCount = 0;
bool event = false;
bool reported = false;
bool gettingWindow = false;
int calmCount = 0;

// Pointer for the axies in question. axis what we look at to detect a car, directionAxis is what we
// look at when trying to determine direction
int* axis= &y;
int* directionAxis= &z;


//For median Filter, hold the previous value
int previousValue;
//the window of points for the median filter
int window[3]= {
  0};



bool directionIn;

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

    count++;
    if (count <= CALIBRATION_COUNT) {
      directionSum += *directionAxis;
      sum += *axis;
      if (count == CALIBRATION_COUNT) {
        average = sum / CALIBRATION_COUNT;
        directionAverage = directionSum / CALIBRATION_COUNT;
      }

    }


    if ((abs(*axis - average) > THRESHOLD)) {
      //If there hasn't been a car even reported and we haven't filled the window with data
      //start collection window data
      if(window[2]==0 && !reported){
        gettingWindow = true;
      } 
      else{
        gettingWindow = false;
      }

      //If we haven't reported an event and were not collection window data
      if (!reported && !gettingWindow) {
        //sort the window so window[1] is median
        for(int i = 0; i < 3; i++){
          int smallestNumber = window[i];
          int index = i;
          for(int j = i; j < 3; j++){
            if(window[j] < smallestNumber){
              smallestNumber = window[j];
              index = j;
            }
          }
          int temp = window[index];
          window[index] = window[i];
          window[i] = temp;
        }
        //window[1] is the median now, use this to check for direction
        if (window[1] > directionAverage) {
          Serial.print("CAR IN");
        } 
        else {
          Serial.print("CAR OUT");
        }
        //mark event as recorded and clear window
        reported = true;
        window[0] = 0;
        window[1] = 0;
        window[2] = 0;
      } 
      else if(gettingWindow) {
        //Were collection window data now, fill the first 0 entry encontured. 
        if(window[0] == 0){
          window[0] = previousValue;
        } 
        else if (window[1] == 0){
          window[1] = *directionAxis;
        } 
        else if (window[2] == 0){
          window[2] = *directionAxis;
        }
      }

      event = true;
      detectorCount++;
      calmCount = 0;
    } 
    else if (event) {
      calmCount++;
    } 
    else {
      average = (1 - ALPHA) * average + *axis * ALPHA;
      directionAverage = (1 - ALPHA) * directionAverage + *directionAxis * ALPHA;
    }

    if (calmCount >= TIMEOUT) {
      event = false;
      detectorCount = 0;
      calmCount = 0;
      reported = false;
    }

    previousValue = *directionAxis;

  }

  delay(40);
}









