//******************************
// updated 24.01.2020
//
//******************************
#define DEBUG 1
#define READINGTIME 50
#define SERIALSPEED 57600
// #define SERIALSPEED 38400

#include <SoftwareSerial.h> 
#include "globals.h"
#include "ArduCarFct.h"

SoftwareSerial BTSerial(2,3); //(RX,TX)
SoftwareSerial NanoSerial(5,6); //

int Vmax;
int AutoMode; //1: autonomous, 0: bluetooth controlled

int ENA; //PWM
int in1;
int in2;
int in3;
int in4;
int ENB; //PWM

//
int ENAValue;
int ENBValue;
int in1value, in2value, in3value, in4value;

int pinCheck = 13;

BTCommand command, BluetoothCommand, sensorCommand;
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
String tempString, sensorString;
unsigned int distanceL, distanceR, distanceC;
unsigned long mainLoopTime=0;
int wL, wC, wR; //wheights for each direction
int wX, wY;



/**************************************************************************************
  Setup

**************************************************************************************/


void setup()  
{ 
  ENA = 6; //PWM
  in1 = 7;
  in2 = 8;
  in3 = 9;
  in4 = 10;
  ENB = 11; //PWM 

  Vmax = 255;
  AutoMode = 0;   //1: autonomous, 0: bluetooth controlled
  
  pinCheck = 13;

  xAcc = 0;
  yAcc = 0;
  xPad = 0;
  yPad = 0;
  xValue = 0;
  yValue = 0;

  BTCommandTime = 0;


  BTString="";
  NanoString = "";

  debugString = "";
  useAcc = 0;
  mainLoopTime=0;
  // TODO you setup code 
  pinMode(ENA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  Serial.begin(SERIALSPEED);
  BTSerial.begin(SERIALSPEED);
  BTSerial.setTimeout(301);
  NanoSerial.begin(SERIALSPEED);
  NanoSerial.setTimeout(301);
  if (Serial) Serial.println("Starting");
  
  BTCommandTime = millis();
  
  newCommand = 0;
  newSensorValues = 0;
  
  //Sends LOW to the NANO pin #5 to set off the test mode
  analogWrite(A0,0);

} 


/**************************************************************************************
  Main loop

**************************************************************************************/



void loop()  
{  
  //int stringDone=1;
  mainLoopTime = millis();


  //////////////////////////////////////////////////////
  // Listen to BT commands:
  BTSerial.listen();
  delay(5); //Needed for stability when switching to new serial port 
  while ((millis()<mainLoopTime+READINGTIME))  //READINGTIME ms to read from bluetooth
  {
    if (BTSerial.available()) {
      BTString = BTSerial.readStringUntil('s');
      BTString+='s';
      BTc = 's';
      // new command received:
      newCommand = 1;
      //Serial.println(BTString);
  
    }
    else BTc = '.';
  }
 

/* DEBUG
  if (Serial)
  {
    Serial.print("\nBT String: "); 
    Serial.print(BTString);
    Serial.print("\tType:");
    Serial.println(BluetoothCommand.commandType);
    Serial.print("Nano String: ");
    Serial.print(NanoString);
    Serial.print("\tType:");
    Serial.println(sensorCommand.commandType);
  }  */
  
  
  ////////////////////////////////////////////////////// 
  if (newCommand)
  {
    //update command time. 
    BTCommandTime = millis();
    
    //Command string should be complete and command must be evaluated.
    BluetoothCommand = getCommandFromString(BTString); 
    //sensorCommand = getCommandFromString(sensorString);

    
    //reset newCommand
    newCommand = 0;    
    
    // String to be sent to the Bluetooth controller (smartphone)
    BTOutputString = "*G";

    //need to evaluate if a switch change happened
    if(BluetoothCommand.commandType=='S') 
    {
      if (BluetoothCommand.x) AutoMode = 1;
      else AutoMode = 0;
    }
         
    
    if (!AutoMode) 
    {      
      //int translateCommandToAcceleration(BTCommand command, int*xValue, int*yValue, String* BTOutputString) 
      if(!translateCommandToAcceleration(BluetoothCommand, &xValue, &yValue, &BTOutputString) )
      {
        //if output = true, it is a test case and we need to re interpret 
        BTString = testMotor();
        BluetoothCommand = getCommandFromString(BTString);
        translateCommandToAcceleration(BluetoothCommand, &xValue, &yValue, &BTOutputString);
        
        Serial.println("Switch: " + sensorCommand.commandType);
      }
      command = BluetoothCommand;
    }
        
    // // // // 
    switch(command.commandType) {
      case 'A':
        xAcc = int(command.x*3); 
        yAcc = int(command.y*3); 
        BTOutputString += command.y;
        break;
      case 'S':
        if (int(command.x)) {
          //useAcc = 1;
          AutoMode=1;
          digitalWrite(pinCheck, HIGH);
        }
        else {
          //useAcc = 0;
          AutoMode=0;
          digitalWrite(pinCheck, LOW);
        }
        BTOutputString += 0;
        break;
      case 'T':
          BTOutputString = "*G";
          BTOutputString += command.x;
          /*if (Serial) {
            Serial.print("BT output string: ");
            Serial.println(BTOutputString);
          }*/
          BTString = testMotor();
          BTc = BTString[BTString.length()-1];
        break;
      case 'P':
        xPad = int(command.x);
        yPad = int(command.y);
        BTOutputString += command.y;
        break;
      case 'O':
        xPad=0;yPad=0;yAcc=0;xAcc=0;
        BTOutputString += 0;
        break;
      case 'V':
        BTString = testMotor();
        BTc = BTString[BTString.length()-1];        
        command = getCommandFromString(BTString);
        xPad = int(command.x);
        yPad = int(command.y);
        BTOutputString += command.y;
        break;
      default:
        BTOutputString += 0;
        Serial.print("Command type: ");
        Serial.print(command.commandType);
        Serial.print("x: ");
        Serial.print(command.x);
        Serial.print(", y: ");
        Serial.print(command.y);
        Serial.println(" :-)");
        return;
    }
    //
    
    if (useAcc) {
      xValue = xAcc;
      yValue = yAcc;
    }
    else {
      xValue = xPad;
      yValue = yPad;
    }  
    
    

    // Debug check:
    /*if (Serial)
    {
      Serial.print("BT Output string: ");
      Serial.print(BTOutputString);
    }  */  
    
    ENAValue = -xValue + yValue;
    ENBValue = +xValue + yValue;
    


/*/    
    
    //ENA motor:
    if (ENAValue > 0){
      if (ENAValue > 254) ENAValue=254;
      analogWrite(ENA,min(Vmax,ENAValue));
      digitalWrite(in1,HIGH);
      digitalWrite(in2,LOW);
    }
    else if (ENAValue < 0){
      ENAValue = -ENAValue;
      if (ENAValue > 254) ENAValue=254;
      analogWrite(ENA,min(Vmax,ENAValue));
      digitalWrite(in1,LOW);
      digitalWrite(in2,HIGH);
    }
    else {
      // Stop if yValue == 0
      analogWrite(ENA,0);
      digitalWrite(in1,LOW);
      digitalWrite(in2,LOW);      
    }
 */
    //ENA motor:
    Vmax = 255;
    if (ENAValue > 0){
      ENAValue = min(Vmax, ENAValue);
      in1value = HIGH;
      in2value = LOW;
    }
    else if (ENAValue < 0){
      ENAValue = -ENAValue;
      ENAValue = min(Vmax,ENAValue);
      in1value = LOW;
      in2value = HIGH;
    }
    else {
      // Stop if yValue == 0
      ENAValue = 0;
      in1value = LOW;
      in2value = LOW;    
    }
        
    //ENB motor
    if (ENBValue > 0){
      ENBValue = min(Vmax,ENBValue);
      in3value = HIGH;
      in4value = LOW;
    }
    else if (ENBValue < 0){
      ENBValue = -ENBValue;
      ENBValue = min(Vmax,ENBValue);
      in3value = LOW;
      in4value = HIGH;
    }
    else {
      // Stop if yValue == 0
      ENBValue = 0;
      in3value = LOW;
      in4value = LOW;
    }  
  } 
  else if (millis()-BTCommandTime > 500) //Stop motor after 500ms without input
  { 
    ENAValue = 0;
    in1value = LOW;
    in2value = LOW;  
    ENBValue = 0;
    in3value = LOW;
    in4value = LOW;
    //if (Serial) Serial.println("Stop");

  }//end if(newCommand)


  Serial.print("Values: ENA:");
  Serial.print(ENAValue);
  Serial.print("\tin1:");
  Serial.print(in1value);
  Serial.print("\tin2:");
  Serial.print(in2value);
  Serial.print("\tENB:");
  Serial.print(ENBValue);
  Serial.print("\tin3:");
  Serial.print(in3value);
  Serial.print("\tin4:");
  Serial.println(in4value);  
    
  //Write values to the motors:
  analogWrite(ENA,ENAValue);
  digitalWrite(in1,in1value);
  digitalWrite(in2,in2value);
  analogWrite(ENB,ENBValue);
  digitalWrite(in3,in3value);
  digitalWrite(in4,in4value);
      
  // Send the string to the Bluetooth controller (smartphone)
  BTOutputString = "*GX:";
  BTOutputString += xValue;
  BTOutputString += " Y:";
  BTOutputString += yValue;
  //BTSerial.print(BTOutputString);  
  Serial.println(BTOutputString); 
  
  
  /**/
  //Serial.print("Time: ");
  //unsigned long delta = millis()-mainLoopTime;
  //int myDelay = 30-delta;
  //Serial.print("delay: ");
  //Serial.println(myDelay);
  //if ((myDelay > 30)|  (myDelay <0) ) myDelay = 0;
  //delay(myDelay);
  
}