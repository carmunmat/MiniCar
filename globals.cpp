#include "globals.h"
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

BTCommand::BTCommand() {
  x = 0;
  y = 0;
  commandType = 'Z';
}

BTCommand::BTCommand (float a, float b, char c){
  x = a;
  y = b;
  commandType = c;
}




/*
 int ENA = 9;
 int in1 = 7;
 int in2 = 8;

 int ENB = 10;
 int in3 = 11;
 int in4 = 12;

//int PWM1 = 5;
//int PWM2 = 6;
//int Serial_RD = 4:
//int Serial_TD = 13;
 int pinCheck = 13;

 BTCommand command;
 int xAcc = 0;
 int yAcc = 0;
 int xPad = 0;
 int yPad = 0;
 int xValue = 0;
 int yValue = 0;

 unsigned long BTCommandTime = 0;

 int newCommand;
 int newSensorValues;

 String BTString="";
 String NanoString = "";
 char NanoChar;
 char BTc;

 String debugString = "";
 int useAcc = 0;
 String BTOutputString;
 int testCase;
 String sensorString;
 unsigned int distanceL, distanceR, distanceC;
 unsigned long mainLoopTime=0;
 int wL, wC, wR; //wheights for each direction
 int wX, wY;
/**/
