//! Arduino Home Energy Monitor
/*!
	Monitors current energy usage and total energy usage
	by counting pulses and time between pulses.

	Circuit:
	* Arduino Yún
	* Light Sensor connected to interrupt 0 (pin 3)
*/

//! Counts the amount of pulses
volatile unsigned long pulseCount = 0;

//! Stores the current usage in Watts for debugging purposes
volatile unsigned long usage = 0;

//! Stores the last time data was send to the filesystem
unsigned long previousMillis = 0;

//! Const interval at which we try to store data
const long interval = 300000;

//! Setup, (set the initial pinMode and) attachInterrupt of the sensor
void setup()
{

}

void loop()
{

}

void pulse()
{

}


