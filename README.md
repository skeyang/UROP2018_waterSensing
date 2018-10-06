# UROP2018_waterSensing

Install Code Composer Studio to program the cc2650 launchpad.

Download SimpleLink Academy and TI BLE SDK from the link below:
http://software-dl.ti.com/lprf/simplelink_academy/overview.html

Import the ADC Ranger BLE project from SimpleLink Academy under Sensor Controller using Resource Explorer Classic. This contains the example code that uses an ultrasonic sensor to read ADC values and send data via bluetooth. Replace the files from the following folders with the one in this github repository:
1. Application
2. Board
3. PROFILES
4. Scif
5. Startup

The ADC sensors can be configured by iniliazing the ADC drivers from ADC.h or using Sensor Controller Studio.
