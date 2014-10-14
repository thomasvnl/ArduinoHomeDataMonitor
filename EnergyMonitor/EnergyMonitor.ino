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
volatile unsigned long currentWattUsage = 0;

//! Stores the last time data was send to the filesystem
unsigned long previousIntervalMillis = 0;

//! Stores the last time a pulse was seen
unsigned long previousPulseMillis = 0;

//! Const interval at which we try to store data
const long interval = 300000;

//! Const amount of pulses per kW
const int amountOfPulsesPerKW = 1000;

//! Setup, (set the initial pinMode and) attachInterrupt of the sensor
void setup()
{
	// Setup Serial connection
	Serial.begin(9600);

	// Attach interrupt to the pulse method
	// Interrupt gets triggered whenever the input is FALLING
	attachInterrupt(0, pulse, FALLING);
}

void loop()
{

}

void pulse()
{
	// Increment the current Pulse Count
	pulseCount++;
	calculateCurrentUsage();
}

void calculateCurrentUsage()
{
	unsigned long currentPulseMillis = millis();
	unsigned long deltaTPulse = currentPulseMillis - previousPulseMillis;
	previousPulseMillis = currentPulseMillis;

	currentWattUsage = ( 3600000 / ( amountOfPulsesPerKW * deltaTPulse * 1000 ) );
	Serial.println(currentWattUsage);
}


