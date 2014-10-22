#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    const int CALIBRATION_COUNT = 40;
    const double ALPHA = 0.005;
    const int THRESHOLD = 8;
    const int TIMEOUT = 20;
		
		// Pointer for the axies in question. axis what we look at to detect a car, directionAxis is what we
		// look at when trying to determine direction
    int* axis;
		int* directionAxis;

		//For median Filter, hold the previous value
		int previousValue;
		//the window of points for the median filter
		int window[3]= {0};

    ifstream in("data/data2.txt");

    int x, y, z;

		//Declare the axies were going to use for magnitude and direction
    axis = &y;
		directionAxis = &z;

		//varibles for algorithm, note any variable prefixed with direction is for the direction axis.

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

    bool directionIn;
    while (in >> x >> y >> z) {
        count++;
        if (count <= CALIBRATION_COUNT) {
						directionSum += *directionAxis;
            sum += *axis;
            if (count == CALIBRATION_COUNT) {
                average = sum / CALIBRATION_COUNT;
								directionAverage = directionSum / CALIBRATION_COUNT;
            }
            continue;
        }


        
        
        if ((abs(*axis - average) > THRESHOLD)) {
						//If there hasn't been a car even reported and we haven't filled the window with data
						//start collection window data
						if(window[2]==0 && !reported){
							gettingWindow = true;
						} else{
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
								cout << directionAverage << " " << window[0] << " " << window[1]<< " " << window[2] << endl;
								//window[1] is the median now, use this to check for direction
                if (window[1] > directionAverage) {
                    cout << "Car in! ----------------------------------------------" << endl;
                } else {
                    cout << "Car out! -----------------------------------------------" << endl;
                }
								//mark event as recorded and clear window
                reported = true;
								window[0] = 0;
								window[1] = 0;
								window[2] = 0;
            } else if(gettingWindow) {
							//Were collection window data now, fill the first 0 entry encontured. 
							if(window[0] == 0){
								window[0] = previousValue;
							} else if (window[1] == 0){
								window[1] = *directionAxis;
							} else if (window[2] == 0){
								window[2] = *directionAxis;
							}
						}
           
            event = true;
            detectorCount++;
            calmCount = 0;
				} else if (event) {
            calmCount++;
        } else {
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

    return 0;
}



//A median filter works by collecting data around the spike of detection, this ensures that we
// grab the data that is most likely not part of the average or an outlier. While a good median 
// filter will use around 9 or 7 elements, because the magnitude can switch so quickly 3 is a 
// good number to use.
