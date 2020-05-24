//#include "ArduCarFct.cpp"
#ifndef ArduCarFct_h
#define ArduCarFct_h

#include "Arduino.h"
#include "globals.h"




/**************************************************************************************
Test the motor pinning

**************************************************************************************/

int testMotorPins(int in1, int in2, int in3, int in4, int ENA, int ENB);

///////////////////////////////////////////////////////
// BTCommand extractCoordinates(String inputString, char cType)
//
// Evaluates if a string has a valid format, which can be:
// 1. "SX<number>*  
// 2. "SX<number>,<number>s"  
// 3. "SPX<number>Y<number>s"
// 4. other format 
// where <number> can be 
// a. ###
// b. ###.###
// 
// If the format is 1 or 2 the <number> values will be 
// written in x, y values of the output.
///////////////////////////////////////////////////////

BTCommand extractCoordinates(String inputString, char cType);


//********************************************************
// BTCommand getCommandFromString(String ReadString)
//
// Translates the received string into a number (integer)
// The received string must have the following format:
// string = "CC<value>s", where:
// CC is a set of 2 characters indicating the type of signal
// - SA: Accelerometer
// - SL: Slider
// - SM: V max
// - SO: STOP signal
// - SP: Pad (SPX__Y__s)
// - SS: Switch
// - ST: String
// - SV: Test case
// * is the string terminator
// <value> is the value to be extracted.
// Return values:
// '!': String not valid, must be resetted
// '0': String incomplete
// 'U': Unknown command
// <int>: value to be translated
//********************************************************

BTCommand getCommandFromString(String ReadString);


/********************************************************
  int translateCommandToAcceleration(BTCommand command,int*xValue,int*yValue, String* BTOutputString)

  takes a command type and translates it to x, y acceleration
  values
  
  Writes on BTOutputString the information sent back to the bluetooth device

  Returns a 0 if it is a normal case
  Returns a 1 if it is a test case (command type V or T)

********************************************************/


int translateCommandToAcceleration(BTCommand command, int*xValue, int*yValue, String* BTOutputString);

/**************************************************************************************
Test the motor

**************************************************************************************/

String testMotor(void);

#endif