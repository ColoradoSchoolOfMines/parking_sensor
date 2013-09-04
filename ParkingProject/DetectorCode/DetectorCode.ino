

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

//#include <EEPROM.h> // include these to store the data in the eeprom, current code does not implement
//#include "EEPROMAnything.h" // include these to store the data in the eeprom, current code does not implement


/* This is an arduino sketch for detecting cars and their direction using the HMC5883 magnetometer sensor.
 This code was developed using data taken 5.10.13 Colorado School of Mines CTLM parking lot.
 
 ******************
 HOW IT WORKS:
 ******************
 
 An array of 'baseline' or nominal values -essentially when the sensor does NOT have a vehicle over it- is gathered initially. 
 This baseline can be 'sized' to allow for fine tuning using the 'baselineSize' variable in the 'Variables you can change' section.
 The standard deviation is developed from this baseline of values to compare future values to. After the initial buffer is filled and the 
 standard deviation is found, The 'window' is built.  The windows uses a moving average algorithm to smooth out noise, to better filter errant data.
 The moving average value is calculated, and compared to the standard deviation.  If it is outside this threshold, a counter is started.  
 Once The counter is larger than the window size ( a very good indication of a car), we have detected a vehicle!  We then grab the last value in 
 the window queue and check to see if it is greater or smaller than the baseline average.  This gives us a direction of the vehicle.  
 
 The code <will be> designed to re-calibrate the 'baseline' every 10 minutes (basically in the event a sensor is hit wiht a solar storm or something).
 
 Currently this only uses the X axis data (as that may be all we need)
 Using datasets taken at the CTLM exits and entrances, it correctly calculates entrances and exits.
 Compiled originally in Sublime 2.0, now in processing with the arduino compiler
 
 **************************************
 Updates
 ------------
 5.24.13 - designed algorithm from data taken from CTLM lot on 5.10.13. Roy Stillwell
 7.9.13 Last update to c++ code. Roy Stillwell
 8.13.13 Added code from c++ algorithm to this arduino sketch. Updated documentation. Roy Stillwell
 *************************************
 */

#include <Wire.h> //I2C Arduino Library, required for interface communication with HMC5883 Device
#include "stats.h"  //a custom library for doing Average and Standard deviation calculations.
#include <cmath>  //Standard cmath library
#include <QueueList.h> // A custom Queue library required for queue manipulation for the running average window.
using namespace std;

//----------Variables you can change--------------------
double recalibrateTime = 600000; //time in seconds. 600000 = 10 min -- used to auto-recalibrate sensor, if time is greater than 10 minutes, recalibrate
#define baselineSize 100  //Size of baseline to use for baseline values
int windowSize = 20;  //Size of 'window' to use for moving average to smooth values. See 'moving averages' in wikipedia
double stdDevThreshold = 6.0; //Used to sort out car 'hits'. Anything above this 'threshold' is counted as a hit
//------------------------------------------------------

long startTime ;                    // start time for the algorithm watch
long elapsedTime ;                  // elapsed time for the algorithm
bool didWeCalculateTheStdDevAlready, saidit = false;
double lastCalibrateTime, stdDev, runningAreaX, runningAreaZ;
double avg = 0;
int x, y, z, movingAveX, lastmovingAvgX;
int windowTotal = 0, count = 0, detectorCount = 0;
int baseline[baselineSize];
QueueList<int> window;

//struct timeval tv;

//#define MEM_SIZE 512 //EEPROM memory size (remaining 2 bytes reserved for count) //needed for storing values in eeprom if needed.  Not currently in use.

#define address 0x1E //0011110b, I2C 7bit address of HMC5883

//int pointer = 0;
//int x,y,z; //triple axis data

void setup(){
  //Initialize Serial and I2C communications
  Serial.begin(57600);
  Wire.begin();

  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

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
   delay(1000);
  // Serial.println('Initializing...');
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
    z = Wire.read()<<8; //Z msb
    z |= Wire.read(); //Z lsb
    y = Wire.read()<<8; //Y msb
    y |= Wire.read(); //Y lsb
  }

  //build the baseline array to be used for standard deviation calculation
  if (count < baselineSize) {
    baseline[count] = x;
    delay(40);
    count++;
  }

  //fill the initial window used for smoothing data
  if (count < windowSize) {
    window.push(x);
    windowTotal += x;
  } 
  else {

    //calculate running average 
    windowTotal -= window.peek();
    windowTotal += x;
    window.push(x);
    window.pop();
    movingAveX = windowTotal / windowSize;

  }

  //Figure out baseline values
  if (count == baselineSize && didWeCalculateTheStdDevAlready == false) {  //The baseline array is full, so we can run the std deviation calculation
    avg = Average(baseline); //get average
    stdDev = Deviation(baseline, avg); //calculate standard deviation of baseline values
    didWeCalculateTheStdDevAlready = true;
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


  bool something = false;


  //Idea: 7.9.13. NOT IMPLEMENTED HERE, JUST A COOLER IDEA.  
  //Take the area under the 'event' curve, and compare that to a known area of a confirmed hit
  // or build a vector of slopes.  compare this vector to a known set of slopes, then if it's close (say two std devs)
  //, count up these 'hits'.  If it crosses a threshold, the overal shape of the graph should match and give positive info.
  //Exactly similar to how if you asked a human to 're-draw' a graph, they would look a the curve, and attempt to replicate
  //it's more predominate features.


  //This algorithm calculates whether 'something' was found, and then to be sure, starts a detectorCount to verify
  //there are enough hits to constitute an actual car passing.
  if ( didWeCalculateTheStdDevAlready == true && (x > (avg + stdDevThreshold) || x < (avg - stdDevThreshold))) {
    something = true;
   // Serial.print('Uh something found, listening...');

    //When 'something' may be out there, there are three scenarios.  
    //1) The detector count goes up if there are less 'something' hits than the window size,
    //2) The detector count hits the windows size -what we qualify as a true car over the sensor!- and as such reports it,
    //3) There wasn't a new event, and the detector count is decremented.  Once it hits zero, the car is thought to have passed the sensor
    // Effectively, while a car is over the sensor, detectorCount stops counting, and sort of cruises at the max window size.  As the car passes
    //and sensor values drop to the nominal baseline, it starts clearing teh detectorCount buffer and readies for a new car.
    if ( detectorCount < windowSize) {
      // 1) Something may be in the works, we need to count the 'something' events to be sure
      detectorCount++;

    } 
    else if (detectorCount == windowSize && saidit==false) {
      // 2) We have officially detected a vehicle!
      saidit = true;
      
      //If the moving average is lower than the baseline average, the car is heading in
      if (movingAveX-avg < 0) {
        Serial.print("time(x40ms): ");
        Serial.print(count); 
        Serial.println(" car heading in!"); 
      } 
      //Otherwise the car was heading out!
      else {
        Serial.print("time(x40ms): ");
        Serial.print(count); 
        Serial.println(" car heading out!"); 
      }
    }
//
    Serial.print(count);
    Serial.print(" avg ");
    Serial.print(avg);
        Serial.print(" stdDev ");
    Serial.print(stdDev);
    Serial.print(" ActualX ");
    Serial.print(x);
    Serial.print(" MovingAveX: ");
    Serial.print(movingAveX);
    Serial.print(" movingAveX-avg ");
    Serial.print(movingAveX-avg);
    
    Serial.print(" detectorCount ");
    Serial.println(detectorCount);
    
    delay(40);
    
    // cout << count << " " << x << " moving ave: "<< movingAveX << " " << 
    // y << " " << z  << " " <<  avg << " " << movingAveX-avg << " " << stdDev << " "<< runningAreaX << " " << 
    // detectorCount << endl;

  } 
  else {
    //3) An event was not detected, so the detectorCount begins decrementing its values
    if (detectorCount > 0) detectorCount--;
    //Once it zero, the algorithm is essentially ready for a new car.
    if (detectorCount == 0) saidit = false;
    
    //legacy c++ code for reference.
    // cout << count << " " << x << " moving ave: "<< movingAveX << " " << 
    // y << " " << z  << " " <<  avg << " " << movingAveX-avg << " " << stdDev << " "<< runningAreaX << " " << 
    // detectorCount << endl;
  }

  //This block of code will print the raw data from the sensors. 
//    Serial.print(x);
//    Serial.print(" ");
//    Serial.print(y);
//    Serial.print(" ");
//    Serial.println(z);
//  
//
//  //add this delay if reading raw data from the serial port. If not, the data comes in way too fast!
//  delay(40);
}








