# UROP2018_waterSensing

Install Code Composer Studio to program the cc2650 launchpad.

Download SimpleLink Academy and TI BLE SDK from the link:
http://software-dl.ti.com/lprf/simplelink_academy/overview.html

Import the ADC Ranger BLE project from SimpleLink Academy under Sensor Controller using Resource Explorer Classic. This contains the example code that uses an ultrasonic sensor to read analog values and send data via bluetooth. Replace the files from the following folders with the one in this github repository:
  1. Application
  2. Board
  3. PROFILES
  4. Scif
  5. Startup

The ADC sensors can be configured by using the ADC driver functions from ADC.h or using Sensor Controller Studio. Sensor Controller Studio generates the code for the sensors and the codes are placed in the Scif folder. In this repository, we used Sensor Controller Studio for our ADC sensors. 

Bluetooth services can be created using the bluetooth service generator located in the link:
http://software-dl.ti.com/lprf/simplelink_academy/modules/ble_01_custom_profile/ble_01_custom_profile.html
