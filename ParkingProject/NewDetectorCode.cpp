#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

int main() {
    const int CALIBRATION_COUNT = 40;
    const double ALPHA = 0.05;
    const int THRESHOLD = 20;
    const int TIMEOUT = 20;

    int* axis;

    ifstream in("data.txt");

    int x, y, z;

    axis = &z;

    int count = 0;
    int sum = 0;
    double average;
    int detectorCount = 0;
    bool event = false;
    bool reported = false;
    int calmCount = 0;

    bool directionIn;
    while (in >> x >> y >> z) {
        count++;
        if (count <= CALIBRATION_COUNT) {
            sum += *axis;
            if (count == CALIBRATION_COUNT) {
                average = sum / CALIBRATION_COUNT;
            }
            continue;
        }
        
        
        if (abs(*axis - average) > THRESHOLD) {
            
            if (!reported) {
                if (*axis > average) {
                    cout << "Car in! ----------------------------------------------" << endl;
                } else {
                    cout << "Car out! -----------------------------------------------" << endl;
                }
                reported = true;
            }
            
            event = true;
            detectorCount++;
            calmCount = 0;
        } else if (event) {
            calmCount++;
            cout << calmCount << endl;
        } else {
            average = (1 - ALPHA) * average + *axis * ALPHA;
        }

        if (calmCount >= TIMEOUT) {
            event = false;
            detectorCount = 0;
            calmCount = 0;
            reported = false;
        }


               
        
    }

    return 0;
}
