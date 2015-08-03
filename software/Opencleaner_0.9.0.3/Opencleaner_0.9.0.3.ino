/* 
Opencleaner Arduino Sketch
Version 0.9.0.3 (beta)
Revised 31 July 2015
https://github.com/epiil/open-cleaner
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

//---------------------------------- Motor shield instantiation -------------------------------------------

// Creates motor shield object with alternate 0x61 I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(0X61); 

// Selects ports for spindle and pellon motors
Adafruit_DCMotor *spindlesupplyMotor = AFMS.getMotor(1);
Adafruit_DCMotor *spindletakeupMotor = AFMS.getMotor(2);
Adafruit_DCMotor *pellonMotor1 = AFMS.getMotor(3);
Adafruit_DCMotor *pellonMotor2 = AFMS.getMotor(4);

//---------------------------------- LCD shield instantiation -------------------------------------------

// Creates LCD shield object
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define BUTTON_SHIFT BUTTON_SELECT

unsigned long lastInput = 0; // last button press
 
const int logInterval = 10000; // log every 10 seconds
long lastLogTime = 0;

//---------------------------------- Pin definitions -------------------------------------------

#define LED 13

//---------------------------------- Button Definitions -----------------------------------

//TBD

//----------------------------------- Variables -------------------------------------------

// Character buffer for serial input/serial monitor
boolean LED_STATE = false;
char COMMAND[30];               

// Motor variables
int spindlesupplyMotorSpeed = 0;
int spindletakeupMotorSpeed = 0;
int pellonMotor1Speed = 0;
int pellonMotor2Speed = 0;

//--------------------------------- General functions -------------------------------------------

// Toggles arduino on-board LED for easy debugging
void toggle_LED(void)  

{
  LED_STATE = !LED_STATE;
  digitalWrite(LED, LED_STATE);
}
//-------------------------------- Serial Port Support ---------------------------------------

void CLEAR_SERIAL_BUFFER(void)
{
 int junk = 0;
  while (Serial.available())
  {
  junk = Serial.read();
  }
}

void SHOW_MOTOR_COMMAND_CHOICES(void)
{
  Serial.println("1 Run spindles FORWARD");
  Serial.println("2 Run spindles BACKWARD");
  Serial.println("3 Brake spindlesupplyMotor");
  Serial.println("4 STOP spindles");
  Serial.println("5 SetspindlesupplyMotor Speed");
  
  Serial.println("6 Run spindletakeupMotor forward at current speed");
  Serial.println("7 Run spindletakeupMotor reverse at current speed");
  Serial.println("8 Brake spindletakeupMotor");
  Serial.println("9 Stop spindletakeupMotor");
  Serial.println("A Set spindletakeupMotor Speed");

  Serial.println("B Run pellonMotor1 forward at current speed");
  Serial.println("C Stop pellonMotor1");
  Serial.println("D Set pellonMotor1 Speed");

  Serial.println("E Run pellonMotor2 forward at current speed");
  Serial.println("F Stop pellonMotor2");
  Serial.println("G Set pellonMotor2 Speed");
}

//-------------------------------- Motor functions -------------------------------------------
// Only two motor-specific functions to call:

// motorname->run(COMMAND)
// where COMMAND is
// FORWARD            Run Forward
// BACKWARD           Run Reverse
// BRAKE              Short the windings together.  Stops the motor as fast as possible.
// RELEASE            Both windings in High impedance state. Free wheeling motor like it is disconnected.

// motorname->setSpeed(SPEED)
// where SPEED is 0 to 255 with 0 as the slowest

void setup() 
{
  Serial.begin(9600);           // set up Serial library at 9600 bps
  
//---------------------------------- Motor Setup --------------------------------------
  
  AFMS.begin();  // Sets default frequency at 1.6KHz
  
  // Default Speeds (OFF)
  spindlesupplyMotor->setSpeed(150);
  spindletakeupMotor->setSpeed(150);
  pellonMotor1->setSpeed(150);
  pellonMotor2->setSpeed(150); 
  // Turn all motor off.  Free wheeling
  spindlesupplyMotor->run(RELEASE);
  spindletakeupMotor->run(RELEASE);
  pellonMotor1->run(RELEASE);
  pellonMotor2->run(RELEASE);
  SHOW_MOTOR_COMMAND_CHOICES();
  
  //-------------------------------- Linear Potentiometer Setup ---------------------
  
 int tensionValue = analogRead(A3); //reads tension data on analog pin 3
  
//---------------------------------- Initialize LCD Display -------------------------

  lcd.begin(16, 2); // Set up LCD's number of columns and rows 
  lcd.setCursor(0,0);
  lcd.setBacklight(WHITE); 
  lcd.print("Opencleaner"); //Opening screen
  lcd.setCursor(0, 1);  // (note: line 1 is the second row, since counting begins with 0):
  lcd.print("Version 0.9.0.3"); 
  delay(5000);
  lcd.clear();
}

uint8_t i=0;
void loop() 
{
   lcd.setBacklight(WHITE);
   uint8_t buttons = lcd.readButtons();

  if (buttons) {
    lcd.clear();
    lcd.setCursor(0,0);
    if (buttons & BUTTON_UP) {
      lcd.print("Stop Tape ");
      spindlesupplyMotor->run(RELEASE);
      spindletakeupMotor->run(RELEASE);
    }
    if (buttons & BUTTON_DOWN) {
      lcd.print("Stop Tape ");
      spindlesupplyMotor->run(FORWARD);
      spindlesupplyMotor->setSpeed(230);
      spindletakeupMotor->run(FORWARD);
      spindletakeupMotor->setSpeed(230);
    }
    if (buttons & BUTTON_LEFT) {
      lcd.print("Reverse Tape ");
      spindlesupplyMotor->run(FORWARD);
      spindlesupplyMotor->setSpeed(210);
      spindletakeupMotor->run(FORWARD);
      spindletakeupMotor->setSpeed(30);
    }
    if (buttons & BUTTON_RIGHT) {
      lcd.print("Forward Tape ");
      spindlesupplyMotor->run(FORWARD);
      spindlesupplyMotor->setSpeed(30);
      spindletakeupMotor->run(FORWARD);
      spindletakeupMotor->setSpeed(210);
    }
    if (buttons & BUTTON_SELECT) {
      lcd.print("SELECT ");
    }
  }

if (Serial.available() > 0) 
  {
  Serial.readBytesUntil('\n',COMMAND,20);// 
  char val = COMMAND[0];
  CLEAR_SERIAL_BUFFER();
  
// ------------------------------------ buttons  ----------------------------------
  
  
// ------------------------------------ spindlesupplyMotor  ----------------------------------
  
  if (val == '1')// spindlesupplyMotor
    {
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("FORWARD ");
    spindlesupplyMotor->run(FORWARD);
     val = 'x';
    spindletakeupMotor->run(FORWARD);
     val = 'x';
    }
  else if (val == '2')// spindlesupplyMotor
    {
	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("BACKWARD ");
    spindlesupplyMotor->run(FORWARD);
     val = 'x';
    spindletakeupMotor->run(FORWARD);
     val = 'x';
    }
  else if (val == '3')// spindlesupplyMotor
    {
    spindlesupplyMotor->run(BRAKE);
     val = 'x';
    }
  else if (val == '4')// spindlesupplyMotor
    {
    spindlesupplyMotor->run(RELEASE);
     val = 'x';
    spindletakeupMotor->run(RELEASE);
     val = 'x';
    }
  else if (val == '5')// spindlesupplyMotor
    {
    Serial.println("Enter speed (0-255)");
    while (!Serial.available())
      { 
        //wait for speed for entry serial port
      }
      delay(10);
      spindlesupplyMotorSpeed  = Serial.parseInt();
      spindlesupplyMotor->setSpeed(spindlesupplyMotorSpeed);
      CLEAR_SERIAL_BUFFER();
      SHOW_MOTOR_COMMAND_CHOICES();
      val = 'x';
    }
	
// ------------------------------------ spindletakeupMotor  ----------------------------------
	
 if (val == '6')// spindletakeupMotor
    {
    spindletakeupMotor->run(FORWARD);
     val = 'x';
    }
  else if (val == '7')// spindletakeupMotor
    {
    spindletakeupMotor->run(BACKWARD);
     val = 'x';
    }
  else if (val == '8')// spindletakeupMotor
    {
    spindletakeupMotor->run(BRAKE);
     val = 'x';
    }
  else if (val == '9')// spindletakeupMotor
    {
    spindletakeupMotor->run(RELEASE);
     val = 'x';
    }
  else if (val == 'A')// spindletakeupMotor
    {
    Serial.println("Enter speed (0-255)");
    while (!Serial.available())
      { 
        //wait for speed for entry serial port
      }
      delay(10);
      spindletakeupMotorSpeed  = Serial.parseInt();
      spindletakeupMotor->setSpeed(spindletakeupMotorSpeed);
      CLEAR_SERIAL_BUFFER();
      SHOW_MOTOR_COMMAND_CHOICES();
       val = 'x';
    }
	
 // ------------------------------------ pellonMotor1  ----------------------------------
	
 if (val == 'B')// pellonMotor1
    {
    pellonMotor1->run(FORWARD);
    }
  else if (val == 'C')// pellonMotor1
    {
    pellonMotor1->run(RELEASE);
    val = 'x';
    }
  else if (val == 'D')// pellonMotor1
    {
    Serial.println("Enter speed (0-255)");
    while (!Serial.available())
      { 
        //wait for speed for entry serial port
      }
      delay(10);
      pellonMotor1Speed  = Serial.parseInt();
      pellonMotor1->setSpeed(pellonMotor1Speed);
      CLEAR_SERIAL_BUFFER();
      SHOW_MOTOR_COMMAND_CHOICES();
       val = 'x';
    }
	
// ------------------------------------ pellonMotor2  ----------------------------------
	
if (val == 'E')// pellonMotor2
    {
    pellonMotor2->run(FORWARD);
     val = 'x';
    }
  else if (val == 'F')// pellonMotor2
    {
    pellonMotor2->run(RELEASE);
     val = 'x';
    }
  else if (val == 'G')// pellonMotor2
    {
    Serial.println("Enter speed (0-255)");
    while (!Serial.available())
      { 
        //wait for speed for entry serial port
      }
      delay(10);
      pellonMotor2Speed  = Serial.parseInt();
      pellonMotor2->setSpeed(pellonMotor2Speed);
      CLEAR_SERIAL_BUFFER();
      SHOW_MOTOR_COMMAND_CHOICES();
       val = 'x';
    }
    
  } //end if (Serial.available() > 0) 
}// end loop
