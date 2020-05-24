#ifndef GLOBALS
#define GLOBALS

#include "Arduino.h"
#include <SoftwareSerial.h> 

/* values for BTCommand::commandType:
  P: Pad, 
  S: Switch, 
  A: Accelerometer; 
  T: String 
  O: Stop
  !: invalid string
  U: unknown command
  0: string incomplete ('s' is missing)
  V: Verification (testing)
*/
 class BTCommand {
  public: 
  float x;
  float y;
  char commandType; 
  BTCommand();
  BTCommand(float, float, char);
};

/*
extern int ENA;
extern int in1;
extern int in2;

extern int ENB;
extern int in3;
extern int in4;

//int PWM1 = 5;
//int PWM2 = 6;
//int Serial_RD = 4:
//int Serial_TD = 13;
extern int pinCheck;

extern BTCommand command;
extern int xAcc ;
extern int yAcc ;
extern int xPad ;
extern int yPad ;
extern int xValue ;
extern int yValue;

extern unsigned long BTCommandTime;

extern int newCommand;
extern int newSensorValues;

extern String BTString;
extern String NanoString;
extern char NanoChar;
extern char BTc;

extern String debugString;
extern int useAcc;
extern String BTOutputString;
extern int testCase;
extern String sensorString;
extern unsigned int distanceL, distanceR, distanceC;
extern unsigned long mainLoopTime;
extern int wL, wC, wR; //wheights for each direction
extern int wX, wY;/**/

#endif