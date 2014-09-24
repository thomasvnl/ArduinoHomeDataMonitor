/*
 * Arduino Home Data Monitor
 * made by Thomas Verschoof
 */

const int interruptNumber = 0;
volatile int pulseCount = 0;

void setup()
{
	// Attach interrupt to countPulse method
	// Is triggered whenever the photosensitive sensor's
	// digital output goes from LOW to HIGH.
	attachInterrupt( 0, countPulse, RISING )
}

void loop()
{
	
}

void countPulse()
{
	pulseCount++;
}