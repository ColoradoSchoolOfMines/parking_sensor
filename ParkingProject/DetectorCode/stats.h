using namespace std;

#include <math.h>
//Returns average value from array
double Average(int window[]) {
	double average = 0;
	for(int i = 0; i < sizeof(window); i++) {
		average += window[i];
	}
	average = average / sizeof(window);
	return average;

}

//does a standard deviation calculation from an array
double Deviation(int window[], double ave)
{
    double E=0;
    int windowSize = sizeof(window) / sizeof(int);
    // Quick Question - Can vector::size() return 0?
    double inverse = 1.0 / windowSize;
    for(unsigned int i=0;i< windowSize;i++)
    {
        E += pow(static_cast<double>(window[i]) - ave, 2);
    }
   // return sizeof(window);
    return sqrt(inverse * E);
}



