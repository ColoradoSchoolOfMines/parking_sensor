#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    const int CALIBRATION_COUNT = 40;
    const double ALPHA = 0.025;
    const int THRESHOLD = 11;
    const int TIMEOUT = 20;

    int* axis;
		int* directionAxis;

		int previousValue;
		int window[3]= {0};

    ifstream in("data.txt");

    int x, y, z;

    axis = &y;
		directionAxis = &z;

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
						if(window[2]==0 && !reported){
							gettingWindow = true;
						} else{
							gettingWindow = false;
						}
            if (!reported && !gettingWindow) {
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
                if (window[1] > directionAverage) {
                    cout << "Car in! ----------------------------------------------" << endl;
                } else {
                    cout << "Car out! -----------------------------------------------" << endl;
                }
                reported = true;
								window[0] = 0;
								window[1] = 0;
								window[2] = 0;
            } else if(gettingWindow) {
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
