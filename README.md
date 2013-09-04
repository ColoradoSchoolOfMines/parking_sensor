parking_sensor
==============

A repository for the parking sensors codebase.

-The DHT_Attiny85 code is for programming an Attiny85 w/ an example DHT 11 humidity/temp sensor

-The ParkingProject codebase is the currently running code on my dev Arduino.  
The DetectorCode is the code currently in development for detecting incoming and outgoing vehicles.
-The RawDataRecorder is code that simply spits out raw data from the HMC5883 sensor to the serial port with a 40ms delay.  It works great for grabbing data and analyzing it in matlab, for example. 

Currently the sensing and transmissions works, up to about 400 feet.  The ParkingProject code is hardly intelligent.

-traffic_counter_software_v5 is a code base pulled from an opensource project off the web http://tomorrow-lab.com
<<<<<<< HEAD
It was code being used for the original pressure sensors we were using.  It is no longer our sensor of choice.

-cpp is c++ code being used to develop sensor detection algorithms.  This code has been ported to the Arduino code now being used in the DetectorCode Model.  


=======
It's being modifed to run our particular setup.

SensorAlgorithm
==============
Author: Roy Stillwell, Colorado School of Mines
5.24.13 - started
7.9.13 Last update 

This is a c++ algorithm for detecting cars using the HMC magnetometer sensor data taken 5.10.13

The script is <will be> designed to re-calibrate the 'baseline' being used as the baseline values every 10 minutes.

There is also a moving average aglorithm to smooth out noise (thanks info Systems!). 

Currently this only uses the X axis data (as that may be all we need)l

Using datasets taken at the CTLM exits and entrances, it can correctly calculate entrances and exits.

Compiled in Sublime 2.0

MatlabScripts
===============
The Matlab scripts help for analysis of data plotted in 3d and 2d, with code for moving average windows as well as commented code for hamming windows if periodic functions start showing up that we can filter the f* out.  :)
>>>>>>> 31912909448d0d11a10fecb7e3ec1b0e3e9a7a5e
