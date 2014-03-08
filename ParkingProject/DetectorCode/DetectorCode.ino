

/*
Arduino, HMC5883, magnetometer, XBee code 
 by: Roy Stillwell, Andrew Wilson
 Colorado School of Mines
 created on: 10.30.13
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
 
 
 ******************
 HOW IT WORKS:
 ******************
 
 An array of 'baseline' or nominal values -essentially when the sensor does NOT have a vehicle over it- is gathered initially. 
 This baseline can be 'sized' to allow for fine tuning using the 'baselineSize' variable in the 'Variables you can change' section.
 After the initial buffer is filled, its average is calculated. 
 A sensor value is then read and compared to the average and a threshold.  If it is outside this threshold, a counter is started.  
 Once The counter is larger than the window size ( a very good indication of a car), we have detected a vehicle! 
 We then grab the last three values in the window and check to see if it is greater or smaller than the baseline average.  
 This gives us a direction of the vehicle.  
 
 The code <will be> designed to re-calibrate the 'baseline' every 10 minutes (basically in the event a sensor is hit wiht a solar storm or something).
 
 Currently this only uses the Y axis data (as that may be all we need)
 Using datasets taken at the CTLM exits and entrances, it correctly calculates entrances and exits.
 
 */

#include <Wire.h> //I2C Arduino Library, required for interface communication with HMC5883 Device
#include "stats.h"  //a custom library for doing Average and Standard deviation calculations.
#include <cmath>  //Standard cmath library
using namespace std;
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


//----------Variables you can change--------------------
double recalibrateTime = 600000; //time in seconds. 600000 = 10 min -- used to auto-recalibrate sensor, if time is greater than 10 minutes, recalibrate
#define baselineSize 100  //Size of baseline to use for baseline values
#define windowSize 30  //Size of 'window' to use for previous values of the sensor to be considered in calculating a positive detection.
#define windowConsidered 3 //Consider the first n numbers in window to determine direction
double carThreshold = 15.0; //Used to sort out car 'hits'. Anything above or below this 'threshold' is counted as a hit
double pingTime = 60; //(in seconds) Used to make sure sensor is still alive after specified time
//------------------------------------------------------


const int ledPin = 13;
const int XBeeSleep = 2;               // Connect to XBee DTR for hibernation mode
const int waitPeriod = 1;              // Number of 8 second cycles before waking
// up XBee and sending data (8*8 = 64 seconds)
// Variable Definition

float Vcc = 0.0;
float Temp = 0.0;

double pingCounter;    //Used to count cycles so system can 'ping' basestation every 30 seconds
int localCarCount=0;
long startTime ;                    // start time for the algorithm watch
long elapsedTime ;                  // elapsed time for the algorithm
bool didWeCalculateAveDevAlready, saidit = false;
double lastCalibrateTime, stdDev;
double baselineAvg = 0;
float calibrationPercentDone;
int x, y, z;
int windowTotal = 0, count = 0, detectorCount = 0;
int baseline[baselineSize];
int window[windowSize];
int windowx[windowSize];
int windowz[windowSize];

#define address 0x1E //0011110b, I2C 7bit address of HMC5883


// See: http://code.google.com/p/tinkerit/wiki/SecretVoltmeter
float readVcc() {
  signed long resultVcc;
  float resultVccFloat;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(10);                           // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                 // Convert
  while (bit_is_set(ADCSRA,ADSC));
  resultVcc = ADCL;
  resultVcc |= ADCH<<8;
  resultVcc = 1126400L / resultVcc;    // Back-calculate AVcc in mV
  resultVccFloat = (float) resultVcc / 1000.0; // Convert to Float
  return resultVccFloat;
}

// See: http://code.google.com/p/tinkerit/wiki/SecretThermometer
float readTemp() {
  signed long resultTemp;
  float resultTempFloat;
  // Read temperature sensor against 1.1V reference
  ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(MUX3);
  delay(10);                           // Wait for Vref to settle
  ADCSRA |= _BV(ADSC);                 // Convert
  while (bit_is_set(ADCSRA,ADSC));
  resultTemp = ADCL;
  resultTemp |= ADCH<<8;
  resultTempFloat = (float) resultTemp * 0.9338 - 312.7; // Apply calibration correction (Roy S)
  resultTempFloat = resultTempFloat * 1.8 + 32.0;  // Convert to F
  return resultTempFloat;
}


void configureSensor() {
  Wire.begin(); //Initalize I2C interface in Arduino

  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

  //Crank the speed up to 75Hz read speeds (default is 15Hz)
  Wire.beginTransmission(address);
  Wire.write((byte) 0x00);
  Wire.write((byte) 0x18); //this jumps it to 75Hz
  Wire.endTransmission();
  delay(5);

  //Tell the HMC5883 where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

  //Read data from each axis, 2 registers per axis (helps initialize readings)
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8; //X msb
    x |= Wire.read(); //X lsb
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }
  //Pause 
  delay(1000);

}

void setup(){
  //Initialize Serial speed
  Serial.begin(57600);


  configureSensor();

}

//The main loop to repeat indefinitely
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
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }

//Get temp and voltage data
Vcc = (float) readVcc();
Temp = (float) readTemp(); //

  //build the baseline array to be used for average calculation
  if (count < baselineSize) {
    baseline[count] = y;
    if (count == 0) {
      Serial.println("Calibrating...");
    }
      //Echo out to serial how far we are in calibrating.
      //    double calibrationPercentDone =  baselineSize;
      //    calibrationPercentDone = count / calibrationPercentDone * 100;
      //    Serial.print("Hang on, Calibrating ");
      //    Serial.print(calibrationPercentDone);
      //    Serial.print("% done. ");
      //    Serial.print(x);
      //    Serial.print(" ");
      //    Serial.print(y);
      //    Serial.print(" ");
      //    Serial.println(z);

      delay(40);  //Pause for a bunch of cycles so the values are collected over a few seconds.
      count++; //increment counter to fill buffer of size 'baselineSize'
      
      if (count==baselineSize) {
        Serial.println("Done. Ready to take data.");
      }
    }

  

  //Figure out baseline values
  if (count == baselineSize && didWeCalculateAveDevAlready == false) {  //The baseline array is full, so we can begin collecting data!
    baselineAvg = Average(baseline); //get average
    
    didWeCalculateAveDevAlready = true;
    startTime = millis(); 
  }

//  elapsedTime = millis() - startTime;
//  //TODO: write recalibration code
//  if (elapsedTime > recalibrateTime) {
//    //look to recalibrate by building a new temp array
//
//    //check standard deviation of new array against current standard deviation
//    //if within limits, use the new calibration data
//    Serial.println("Uh hang on a minute, recalibrating.");
//    startTime = millis();
//    count = 0;
//  }


  bool something = false;  //used to flag whether or not something is over the sensor

  //This algorithm calculates whether 'something' was found, and then to be sure, starts a detectorCount to verify
  //there are enough hits to constitute an actual car passing, and not some random flux in the Earth's field.
  if ( didWeCalculateAveDevAlready == true && (y >= (baselineAvg + carThreshold) || y <= (baselineAvg - carThreshold))) {

    something = true;  //something is probably out there!

    delay(14); //The HMC is set to run at 75Hz, this delays just that amount!

    //When 'something' may be out there, there are three scenarios.  
    //1) The detector count goes up if there are less 'something' hits than the window size,
    //2) The detector count hits the windows size -what we qualify as a true car over the sensor!- and as such reports it,
    //3) There wasn't a new event, and the detector count is decremented.  Once it hits zero, the car is thought to have passed the sensor
    // Effectively, while a car is over the sensor, detectorCount stops counting, and sort of cruises at the max window size.  As the car passes
    //and sensor values drop to the nominal baseline, it starts clearing the detectorCount buffer and readies for a new car.

    // 1) Something may be in the works, we need to count the 'something' events to be sure and store it in the window
    // We will use the detectorCount as the indexer
    if (detectorCount <= windowSize) {
      window[detectorCount] = y;  //We add the current value to the window, for analysis later
      windowx[detectorCount] = x;
      windowz[detectorCount] = z;
    }
    detectorCount++;  

    if (detectorCount >= windowSize && saidit==false) { //here we check to see if we have detected enough events
      //We did detect a car! Now to see direction
      double windowAve;

      for (int i=0; i < windowConsidered; i++) { //using the first few values of the window (the orignal direction of the Magfield)
        //we find the direction.
        windowAve += window[i]; //add up all the values
      }
      windowAve = windowAve / windowConsidered;  //generate the average value to be used

        //If the first value is less than the average, we have a car heading in! So transmit!
      if (windowAve < baselineAvg  ) {
        localCarCount++;     
        // Serial.print("Car heading in! localCarCount:"); 
        Serial.print(localCarCount); //output car count
        Serial.print(" ");
        Serial.print(Vcc); //output battery voltage (in theory)
        Serial.print(" ");
        Serial.println(Temp); //output device temp (in theory)
        
        //TODO Calibrate temp
        //TODO verify vcc output is correct


        delay(500);
        pingCounter = 0; // Device has communicated with the base station, so reset counter for communication
      } 
      else {  //Otherwise the car was heading out!
        localCarCount--;
        // Serial.print(" car heading out: localCarCount:"); 

        Serial.print(localCarCount); //output car count
        Serial.print(" ");
        Serial.print(Vcc); //output battery voltage (in theory)
        Serial.print(" ");
        Serial.println(Temp); //output device temp (in theory)
        
        //TODO Calibrate temp
        //TODO verify vcc output is correct

        //Serial.print ("Data: ");
        delay(500);
        pingCounter = 0; // Device has communicated with the base station, so reset counter for communication
      }
      //For debugging, we echo the window average and baseline average to makes sure the algorithm is working
      //        Serial.print("windowAve: ");
      //        Serial.print(windowAve);
      //        Serial.print(" baselineAvg: ");
      //        Serial.println (baselineAvg);
      saidit = true; //mark that we have said there is a car already, so we don't repeat it
    }

  } 
  else {
        //3) An event was not detected, so the detectorCount begins decrementing its values
            if (detectorCount > 0) detectorCount--;
            //Once it zero, the algorithm is essentially ready for a new car.
            if (detectorCount == 0) saidit = false;
        
            //This code does is used for diagnostics.  It essentially sends a 'heartbeat' to the base station
            if (pingCounter >= (pingTime*200)) { // is about 1 second with this program code 
              //To be sure everything is working (mostly for diagnostics, ping every 30 seconds).

              Serial.print("All_Still_Quiet ");
              Serial.print(localCarCount); //output car count
              Serial.print(" ");
              Serial.print(Vcc); //output battery voltage (in theory)
              Serial.print(" ");
              Serial.println(Temp); //output device temp (in theory)
              
              pingCounter = 0;
              //delay(40);
         }
         pingCounter++;  
  }

}











