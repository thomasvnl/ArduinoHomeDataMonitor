/*!
	Monitors current energy usage and total energy usage by counting the amount
	of pulses is receives via a photosensitive sensor that is connected to
	intterupt(0).

	Circuit:
	-- Arduino Yun
	-- Photosensitive sensor connected to pin 3
*/
#include <FileIO.h>

//! PINS
const int StatusLEDPin = 13;
const int analogInputPin = A0;

//! Keep track of the amount of pulses (Watts) that have triggered
volatile unsigned long pulseCount = 0;

//! Keep track of the current Watt usage
volatile unsigned long currentWattUsage = 0;

//! Keep track of the last time a pulse was seen
unsigned long prevPulseTime = 0;

//! Keep track of the last time data was logged
unsigned long prevDataLogTime = 0;

//! CSV Delimiter used for data logging
const char delim = ',';

//! CSV New Line
const char nl = '\n';

//! File to log to
const char dataFile[ ] = "/mnt/sd/PulseCount.log";

//! Const length of one minute millis
const unsigned long aMinute = 60000;

//! Const interval which defines the amount of time between data logs,
//! is set in (void) setup();
//const unsigned long interval = aMinute * 5;
const unsigned long interval = aMinute * 5;

//! Const amount of pulses per kWh
const unsigned int pulsesPerKWH = 1000;

//! Current Light value
unsigned int currentLightStrength = 0;

//! Setup the interrupt and define minutes in interval
void setup(){

	attachInterrupt( 0, handlePulse, FALLING );

	Bridge.begin();
	Console.begin();
	FileSystem.begin();

	//while(!Console);
	//Console.println("Started logging at " + getTimestamp() );
}

void loop(){
	readCurrentLightStrength();

	unsigned long curDataLogTime = millis();

	if( curDataLogTime - prevDataLogTime > interval )
	{
		prevDataLogTime = curDataLogTime;
		Console.println( getTimestamp() + ">> Pulses: " +  String(pulseCount) + "; Current watt usage: " + String( calculateIntervalWattUsage() ) );
		String data = String(currentLightStrength) + delim + String( pulseCount ) + delim + String( calculateIntervalWattUsage() );

		if( !writeToFile( data ) ){
			Console.println( "Could not write to file :'( " );
		}
                else
                {
                   pulseCount = 0; 
                }
	}
}

void handlePulse()
{
	unsigned long curPulseTime = millis();
	pulseCount++;

	if( prevPulseTime != 0 ){
		calculateCurrentWattUsage( curPulseTime );
		//Serial.println( "DeltaTPulseTime: " + String( curPulseTime - prevPulseTime ) );
		// Can't do this, interrupt inside an interrupt freezes arduino; //Console.println( "New Watt usage: " + String( currentWattUsage ) );
	}

	prevPulseTime = curPulseTime;
}

void calculateCurrentWattUsage( unsigned long curPulseTime )
{
	unsigned long deltaTPulseTime = curPulseTime - prevPulseTime;
	currentWattUsage = ( 3600000.0 / ( pulsesPerKWH * deltaTPulseTime ) ) * 1000.0;
}

unsigned long calculateIntervalWattUsage()
{
 unsigned long meanWatt;
 meanWatt = ( pulseCount * 3600000 ) / interval; 
 return meanWatt;
}

void readCurrentLightStrength()
{
	// Rectify measurement of sensor
	currentLightStrength = ( 1023 - analogRead( analogInputPin ) );
}

void activeLED(){
	digitalWrite( StatusLEDPin, HIGH );
}

void inactiveLED(){
	digitalWrite( StatusLEDPin, LOW );
}

bool writeToFile( String data )
{
	File fh = FileSystem.open( dataFile, FILE_APPEND );
	if( !dataFile ) return false;
	
	activeLED();

	data = getTimestampZeroSec() + delim + data + nl;
	fh.print( data );
	fh.close();

	inactiveLED();
	
	//Serial.println( "Wrote to log" );

	return true;
}

String getTimestampZeroSec()
{
	String result = getTimestamp();
	result = result.substring(0, result.length() - 2 );
	result += String("00");
	return result;
}

String getTimestamp(){
	String result;
	Process time;

	time.begin("date");
	time.addParameter("+%Y-%m-%d %T");
	time.run();

	while( time.available() > 0 ){
		char c = time.read();
		if( c != '\n' ){
			result += c;
		}
	}

	return result;
}
