ArduinoHomeDataMonitor
======================

Monitoring different household data with an Arduino Yún

Note: You might not need all hardware and software modules if you don't want to implement all features.

Hardware prerequisits:
+ Arduino Yún
+ Photosensitive Sensor Module for catching the "light pulse" output from a modern electricity meter (I used: http://www.dx.com/p/diy-photosensitive-diode-module-blue-black-200148 )
+ DHT11 Temperature & Humidity Sensor Module ( I used: http://www.dx.com/p/arduino-digital-temperature-humidity-sensor-module-121350 )

Software prerequisits on the Arduino side
+ DHT11 Library ( I used: https://github.com/markruys/arduino-DHT )

Software prerequisits on the Linux side
+ Python
+ MySQL
+ Python MySQL connector
+ nmap ( for monitoring active network devices based on mac address )