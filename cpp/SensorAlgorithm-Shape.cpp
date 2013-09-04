//SensorAlgorithm-Shape.cpp
//This is a c++ algorithm for detecting cars using the HMC magnetometer sensor data taken 5.10.13
//The script is <will be> designed to re-calibrate the 'baseline' being used as the baseline values every 10 minutes.
//There is also a moving average aglorithm to smooth out noise. Currently this only uses the X axis data (as that may be all we need)
//Using datasets taken at the CTLM exits and entrances, it correctly calculates entrances and exits.

//Idea: 6.9.13.  Take the area under the 'event' curve, and compare that to a known area of a confirmed hit
// or build a vector of slopes.  compare this vector to a known set of slopes, then if it's close (say two std devs)
//, count up these 'hits'.  If it crosses a threshold, the overal shape of the graph should match and give positive info.
//Exactly similar to how if you asked a human to 're-draw' a graph, they would look a the curve, and attempt to replicate
//it's more predominate features.
//Compiled in Sublime 2.0

//Author: Roy Stillwell, Colorado School of Mines
//6.14.13 


#include <iostream>
#include <fstream>
#include "stats.h"
#include <time.h>
#include <cmath>
#include <queue>
using namespace std;
//----------Variables you can change--------------------
   double recalibrateTime = 600; //time in seconds 600 = 10 min, used to auto-recalibrate sensor
   int baselineSize = 100;  //Size of baseline to use for baseline values
   int windowSize = 20;  //Size of window to use for moving average to smooth values
   double stdDevThreshold = 6.0; //Used to sort out car 'hits' 
//------------------------------------------------------

int main ()
{
  cout << "Testing Data!" << endl;
  //std::ifstream infile("5.10.13.NorthCTLMEXIT.txt");
  std::ifstream infile("5.10.13.SouthCTLMEXIT.txt");
  

  bool didWeCalculateTheStdDevAlready, saidit = false;
  double lastCalibrateTime, stdDev, runningAreaX, runningAreaZ;
  double avg = 0;
  int x, y, z, movingAveX, lastmovingAvgX;
  int windowTotal = 0, count = 0, detectorCount = 0;
  int baseline[baselineSize];
  std::queue<int> window;
  std::vector<> vectorOfShitIveSeen;
  std::vector<int> tempv;


  while (infile >> x >> y >> z)
  {
  	//build the baseline array to be used for standard deviation calculation
  	if (count < baselineSize) {
  			baseline[count] = x;
    }

    //fill the initial window used for smoothing data
    if (count < windowSize) {
      window.push(x);
      windowTotal += x;
    } else {
   	  //Window is big enough. 	
      //calculate running average 
      windowTotal -= window.front();
      windowTotal += x;
      window.push(x);
      window.pop();
      lastmovingAvgX = movingAveX;
      movingAveX = windowTotal / windowSize;



      // cout << "window size: " << window.size() << " " << window.front() << " " << window.back()  << " " << movingAveX << endl;

    }

    //Figure out baseline values
	  if (count == baselineSize && didWeCalculateTheStdDevAlready == false) {  //The baseline array is full, so we can run the std deviation calculation
	  		avg = Average(baseline); //get average
	  		stdDev = Deviation(baseline, avg); //calculate standard deviation of baseline values
        didWeCalculateTheStdDevAlready = true;
	  }
  	
    //TODO: write recalibration code
  	if (lastCalibrateTime > recalibrateTime) {
      //look to recalibrate by building a new temp array

      //check standard deviation of new array against current standard deviation
      //if within limits, use the new calibration data


  		count = 0;
  	}


    bool car = false;

    //Algorithm to Calculate if car was found


    //This algorithm calculates an in or an out, and based on this
    if ( didWeCalculateTheStdDevAlready == true && (movingAveX > (avg + stdDevThreshold * stdDev) || movingAveX < (avg - stdDevThreshold * stdDev))) {
      car = true;
      //Build detection array



      //The count detects how many hits were recorded, once it's above 10, we count it as a positive record
        if ( detectorCount < windowSize) {
          detectorCount++;

        } else if (detectorCount == windowSize && saidit==false) {
          saidit = true;
          cout << "car officially here." << endl;
          std::queue<int> tempwindow = window;
          for(i=0;i < wwindowSize; i++) {
 
          }

          //logic to determine direction
          if (movingAveX-avg < 0) {
            cout << "time(x40ms):" <<count << " car heading in!" << endl;
          } else {
          cout << "time(x40ms):" << count << " car heading out!" << endl;
          }
        }


      cout << count << " " << x << " moving ave: "<< movingAveX << " " << 
      y << " " << z  << " " <<  avg << " " << movingAveX-avg << " " << stdDev << " "<< runningAreaX << " " << 
      detectorCount << endl;

    } else {
      if (detectorCount > 0) { 
      	detectorCount--;
      	      cout << count << " " << x << " moving ave: "<< movingAveX << " " << 
      y << " " << z  << " " <<  avg << " " << movingAveX-avg << " " << stdDev << " "<< runningAreaX << " " << 
      detectorCount << endl;
      	if (detectorCount==0) {
      		car = false;
      		cout << "car left" << endl;

      	}
      }
      if (detectorCount == 0) {
      	saidit = false;}

    }
    

  	count++;

  }
  
  return 0;
}

