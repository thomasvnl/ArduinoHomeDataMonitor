//! EnergyMonitor.ino
/**!
	Made for use with Arduino Yún

	Monitors current and total energy usage by counting pulses. These pulse can 
	arrise in the form of light- or electic pulses, depending on your setup. 
	This setup relies on a light sensor that outputs 0 when light is detected
	and 1 when no light is detected. The digital output of this sensor is 
	connected to Interrupt(0) of the Arduino Yún.

*/

#include <FileIO.h>
#include <Console.h>
#include <Process.h>

//! Define the used pins (and mode)
const int LEDPin = 13;
const int analogInputPin = A0;
const int interruptPin = 0;
const int interruptMode = FALLING;

//! Keep track of the amount of pulses (W/h) that were measured
volatile unsigned long pulseCount = 0;

//! Keep track of the current energy usage (in W, between pulses)
volatile unsigned long currentWattUsage = 0;

//! Handle with interrupts correctly (Console), this is needed
volatile bool currentUsageHasNewValue = false;

//! Keep track of the last time a pulse was measured
unsigned long mPrevPulseTime = 0;

//! Keep track of the last time the data was "logged"
unsigned long mPrevDataLogTime = 0;

//! The length of one minute in milliseconds
const unsigned long mMinute = 60000;

//! The length of one hour in milliseconds
const unsigned long mHour = mMinute * 60;

//! Interval to use between logging data
const unsigned long mInterval = mMinute * 5;

//! Amount of pulses per kWh (as provided by your digital power meter)
const unsigned int pulsesPerKWH = 1000;

//! The datafiles to write to
const char wattUsageFile[ ] = "/mnt/sd/EnergyMonitor/wattUsage.csv";
const char wattHourFile[ ] = "/mnt/sd/EnergyMonitor/wattHour.csv";

//! Standard characters for CSV files
const char delimiter = ',';
const char endline = '\n';

//! Delimiter for the console output
const String consoleDelim = " >> ";

//! Setup the interrupt and connection to Bridge/Console/FileSystem
void setup()
{
	LEDSwitchOn();
	// Delay neccesarry to wait for Bridge (in some cases).
	delay(2500);

	attachInterrupt( 0, handleNewPulse, interruptMode );

	Bridge.begin();
	Console.begin();
	FileSystem.begin();

	LEDSwitchOff();

}

//! The well known Arduino loop-da-loop loop 
void loop()
{
	unsigned long mCurDataLogTime = millis();

	if( mCurDataLogTime - mPrevDataLogTime > mInterval )
	{
		mPrevDataLogTime = mCurDataLogTime;

		String data;
		String timestamp = getTimestamp();
		unsigned long intervalWattUsage = getIntervalWattUsage();

		Console.println(timestamp + consoleDelim + "Interval Usage (W): " + String(intervalWattUsage) );
		data = timestamp + delimiter + String(intervalWattUsage) + endline;
		if( !writeDataToFile( wattUsageFile, data ) )
		{
			Console.println( getTimestamp() + consoleDelim + "Could not write Interval Watt Usage to file :(!");
		}

		Console.println(timestamp + consoleDelim + "W/h used: " + String(pulseCount ) );
		data = timestamp + delimiter + String(pulseCount) + endline;
		if( !writeDataToFile( wattHourFile, data ) )
		{
			Console.println( getTimestamp() + consoleDelim + "Could not write Watt/Hour Usage to file :(!");
		}
		else
		{
			pulseCount = 0;
		}
	}

	if( currentUsageHasNewValue )
	{
		currentUsageHasNewValue = false;
		Console.println( getTimestamp() + consoleDelim + "Current Usage (W): " + String( currentWattUsage ) );
	}

}

//! Counts the pulse and calls calculateCurrentWattUsage with the current millis
//! if their has been a previous pulse.
void handleNewPulse()
{
	unsigned long mCurPulseTime = millis();
	pulseCount++;

	if( mPrevPulseTime != 0 )
	{
		calculateCurrentWattUsage( mCurPulseTime );
	}

	mPrevPulseTime = mCurPulseTime;
}

//! Method that calculates the current watt usage by using the time between 
//! pulses.
void calculateCurrentWattUsage( unsigned long mCurPulseTime )
{
	unsigned long mPulseDeltaTime = mCurPulseTime - mPrevPulseTime;
	currentWattUsage = ( mHour * 1.0 / ( pulsesPerKWH * mPulseDeltaTime ) ) * 1000.0;
	currentUsageHasNewValue = true;
}

//! Method that returns the interval watt usage (may differ from above). Is less
//! prone to irregularities thus may not show all information about the "current"
//! usage.
unsigned long getIntervalWattUsage()
{
	unsigned long watts = ( pulseCount * mHour ) / mInterval;
	return watts;
}

//! Get the timestamp from the Linux system (NTP keeps it right). Returns the 
//! date and time in Y-m-d H:M:S format, because that makes sense.
String getTimestamp()
{
	String result;
	Process p;

	p.begin("date");
	p.addParameter("+%Y-%m-%d %T");
	p.run();

	while( p.available() > 0 )
	{
		char c = p.read();
		if( c != '\n' )
		{
			result += c;
		}
	}

	return result;
}

//! Helper method to switch the status LED on (LEDPin)
void LEDSwitchOn()
{
	digitalWrite( LEDPin, HIGH );
}

//! Helper method to switch the status LED off (LEDPin)
void LEDSwitchOff()
{
	digitalWrite( LEDPin, LOW );
}

//! Method that writes to a given file
bool writeDataToFile( const char* fileName, String data )
{
	File fileHandler = FileSystem.open( fileName, FILE_APPEND );
	if( !fileHandler ) return false;

	LEDSwitchOn();
	LEDSwitchOff();
	LEDSwitchOn();
	fileHandler.print(data);
	fileHandler.close();
	LEDSwitchOff();
}