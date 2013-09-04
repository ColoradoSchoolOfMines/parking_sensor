parking_sensor
==============

A repository for the parking sensors codebase.

-The DHT_Attiny85 code is for programming an Attiny85 w/ an example DHT 11 humidity/temp sensor

-The ParkingProject codebase is the currently running code on my dev Arduino.  
The DetectorCode is the code currently in development for detecting incoming and outgoing vehicles.
-The RawDataRecorder is code that simply spits out raw data from the HMC5883 sensor to the serial port with a 40ms delay.  It works great for grabbing data and analyzing it in matlab, for example. 

Currently the sensing and transmissions works, up to about 400 feet.  The ParkingProject code is hardly intelligent.

-traffic_counter_software_v5 is a code base pulled from an opensource project off the web http://tomorrow-lab.com
It was code being used for the original pressure sensors we were using.  It is no longer our sensor of choice.

-cpp is c++ code being used to develop sensor detection algorithms.  This code has been ported to the Arduino code now being used in the DetectorCode Model.  


