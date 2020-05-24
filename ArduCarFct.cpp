#include "ArduCarFct.h"


#include <SoftwareSerial.h> 


/**************************************************************************************
Test the motor pinning
**************************************************************************************/

int testMotorPins(int in1, int in2, int in3, int in4, int ENA, int ENB) {
  //init
  
  //ENA
  //Serial.println ("Test ENA: forward");
  digitalWrite(in1,HIGH);
  digitalWrite(in2, LOW);
  analogWrite(ENA,50);
  delay(2000);
  
  //Serial.println ("Test ENA: backward");
  digitalWrite(in2,HIGH);
  digitalWrite(in1, LOW);
  analogWrite(ENA,50);
  delay(2000);

  //Serial.println ("Test ENB: forward");
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4, LOW);
  analogWrite(ENB,50);
  delay(2000);

  //Serial.println ("Test ENB: backward");
  digitalWrite(in4,HIGH);
  digitalWrite(in3, LOW);
  analogWrite(ENB,50);
  delay(2000);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

}



///////////////////////////////////////////////////////
// BTCommand extractCoordinates(String inputString, char cType)
///////////////////////////////////////////////////////

BTCommand extractCoordinates(String inputString, char cType) {
  BTCommand coordinates(0,0,'U');
  int stringLength = inputString.length();
  String auxiliarString = "";

  int commaIndex = 0;
  int commaCount = 0;
  int securityCount = 0;
  
  for (int i=0;i<stringLength;i++) {
    if (inputString[i] == ',') commaCount++;
  }  

  // if cType == 'P', no commas available
  if (cType == 'P') {
    if (commaCount) {
      //Error occured, no commas should be present
      coordinates.commandType = '!';
      return coordinates;
    }
    auxiliarString = inputString.substring(1);
    coordinates.x = auxiliarString.toFloat();
    auxiliarString = auxiliarString.substring(auxiliarString.indexOf('Y')+1);
    coordinates.y = auxiliarString.toFloat();
  }
  else {
    // cType is not 'P'
    if (commaCount >1) {
      coordinates.commandType = '!';
    } 
    else if(commaCount == 0){
      // The string has no comma, but can have one or two values
      // if the string comes from pad, will be SPX##Y##*
      // if thes tring comes from the accelerometer, will be SA#.#s
      coordinates.x = inputString.toFloat();
    } 
    else if (commaCount == 1) {
      // the string has one comma, two values
      coordinates.x = inputString.toFloat();
      auxiliarString = inputString.substring(inputString.indexOf(',')+1, stringLength);
      coordinates.y = auxiliarString.toFloat();
    }
  }
  coordinates.commandType = cType;
  
  return coordinates;
}


//********************************************************
// BTCommand getCommandFromString(String ReadString)
//********************************************************

BTCommand getCommandFromString(String ReadString) {
  String valueString;
  BTCommand outputCommand(0,0,'!');
  
  int StringLength = ReadString.length();
  //check string length. Must be at least 4: "Sx1s"
  if (StringLength<4) return outputCommand;
  
  int count = 0;
  int outputValue=1;

  //Command string must start with an 'S':
  if (ReadString[0] != 'S') {
    //Invalid string, return a '!'
    outputCommand.commandType = '!';
    return outputCommand;
  }
  
  //Command string must end with 's':
  if (ReadString[StringLength-1]!='s') {
    //Incomplete string, return a '0'
    outputCommand.commandType = '0';
    return outputCommand;
  }

  //Analize the command type (second character):
  //Sx....* <--- x?
  valueString = ReadString.substring(2, StringLength-1);
  //write the values into x, y, commandType:
  outputCommand = extractCoordinates(valueString, ReadString[1]);  

  return outputCommand;
}


/********************************************************
  int translateCommandToAcceleration(BTCommand command,int*xValue,int*yValue, String* BTOutputString)
********************************************************/


int translateCommandToAcceleration(BTCommand command, int*xValue, int*yValue, String* BTOutputString) 
{
  int xAcc, yAcc, useAcc, xPad, yPad;
  
  // Depending on the command type, different reactions happen:
  switch(command.commandType) {
    case 'A': //using Accelerometer. Must multiplicate by 3 to convert values
      xAcc = int(command.x*3);
      yAcc = int(command.y*3); 
      // Send y value (as example)
      *BTOutputString += command.y;
      break;
      
    case 'S': // using switch
      if (int(command.x)) {
        useAcc = 1;
        digitalWrite(13, HIGH);
      }
      else {
        useAcc = 0;
        digitalWrite(13, LOW);
      }
      *BTOutputString += 0;
      break;
      
    case 'P': // using the pad as input
      xPad = int(command.x);
      yPad = int(command.y);
      // sending y as example 
      *BTOutputString += command.y;
      break;
      
    case 'O': //Stop command
      xPad=0;yPad=0;yAcc=0;xAcc=0;
      *BTOutputString += 0;
      break;
      
    case 'T':
    case 'V': //for test purposes
      return 0; //We return a 0 to make aware of the special content of the command
      
    default:
      *BTOutputString += 0;
      break;
  }
  
  if (useAcc) {
    xValue = xAcc;
    yValue = yAcc;
  }
  else {
    xValue = xPad;
    yValue = yPad;
  }
  
  return 1;
}

 
/**************************************************************************************
Test the interpretation of the BT command

**************************************************************************************/

int testTranslator(String testString, float x, float y, char c){
  BTCommand command = getCommandFromString(testString);
  int success = 1;
  if( (command.x != x) | (command.y !=  y) | (command.commandType != c) ) success = 0;
  return success;
}


int test(void) {
  String testString;
  String outputString;
  BTCommand command;
  int stringOK = 1;

  //Test 1: input from pad
  Serial.println("Test 1: Pad");
  testString = "SPX0Y12s";
  command = getCommandFromString(testString);
  stringOK = testTranslator(testString, 0, 12, 'P');
  if (!stringOK) {
    outputString = "FAILED: " + testString + " -->" + "x: " + command.x + " y: " + command.y + " C: " + command.commandType;    
    Serial.println(outputString);
  }
  else Serial.println("PASSED");

  //Test 2: input from Accelerometer
  Serial.println("Test 2: Accelerometer");
  testString = "SA0.12,5.6s";
  command = getCommandFromString(testString);
  stringOK = testTranslator(testString, 0.12, 5.6, 'A');
  if (!stringOK) {
    outputString = "FAILED: " + testString + " -->" + "x: " + command.x + " y: " + command.y + " C: " + command.commandType;    
    Serial.println(outputString);
  }
  else Serial.println("PASSED");
  
  //Test 3: input from Accelerometer
  Serial.println("Test 3: Accelerometer");
  testString = "SA12,5.6s";
  command = getCommandFromString(testString);
  stringOK = testTranslator(testString, 12, 5.6, 'A');
  if (!stringOK) {
    outputString = "FAILED: " + testString + " -->" + "x: " + command.x + " y: " + command.y + " C: " + command.commandType;    
    Serial.println(outputString);
  }
  else Serial.println("PASSED");

  //Test 4: input from Switch
  Serial.println("Test 4: Switch");
  testString = "SS0s";
  command = getCommandFromString(testString);
  stringOK = testTranslator(testString, 0, 0, 'S');
  if (!stringOK) {
    outputString = "FAILED: " + testString + " -->" + "x: " + command.x + " y: " + command.y + " C: " + command.commandType;    
    Serial.println(outputString);
  }
  else Serial.println("PASSED");
  
  //Test 6: input from Stop button
  Serial.println("Test 6: Stop");
  testString = "SO1s";
  command = getCommandFromString(testString);
  stringOK = testTranslator(testString, 1, 0, 'O');
  if (!stringOK) {
    outputString = "FAILED: " + testString + " -->" + "x: " + command.x + " y: " + command.y + " C: " + command.commandType;    
    Serial.println(outputString);
  }
  else Serial.println("PASSED");
  
}

/**************************************************************************************
Test the motor

**************************************************************************************/

static unsigned long loopTime=0;

String testMotor(void){

  //unsigned long currentTime = millis();
  int motorIteration = 0;
  String outputString;

  if (millis()-loopTime > 3000){ 
    loopTime = millis();
    
    switch (motorIteration) {
      case 0:
        outputString = "SPX0Y0s";
        Serial.println("stop");
      break;
      case 1:
        outputString = "SPX50Y0s";
        Serial.println("right");
      break;
      case 2:
        outputString = "SPX0Y-50s";
        Serial.println("forward");
      break;
      case 3:
        outputString = "SPX50Y-50s";
        Serial.println("forward + Right");
      break;
      case 4:
        outputString = "SPX0Y0s";
        Serial.println("stop");
      break;
      case 5:
        outputString = "SPX0Y100s";
        Serial.println("backward");
      break;
      case 6:
        outputString = "SPX50Y50s";
        Serial.println("backward right");
      break;
      
      default:
        outputString = "SPX0Y0s";
        motorIteration = -1;
    }
    motorIteration++;
  }
  else outputString = "SP123"; //unfinished string

  return outputString;
  
}

