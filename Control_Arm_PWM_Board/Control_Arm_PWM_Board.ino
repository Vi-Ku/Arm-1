/* **************************************************************************************
 *  NOVA ROVER TEAM - URC2018
 *  This code controls the digital signals, PWM and linear actuators with a PWM feedback loop.
 *
 *Author: James Hain
 ****************************************************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

// PWM board initialisation
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)


//PWM Pins
const int spinPWMPin = 0;
const int lowerPWMPin = 1;
const int upperPWMPin = 2;
const int rollPWMPin = 3;
const int endPWMPin = 4;

const int azimuthPWMPin = 5;
const int horizonPWMPin = 6;

const int endservoPWMPin = 7;

//Arduino Pins
const int lowerPin = 6;           
const int upperPin = 7;           
const int directionLowerPin = 5;  
const int directionUpperPin = 6;
const int directionSpinPin = 4;
const int directionRollPin = 7;


///////////////////////////////////  Variables to change ////////////////////////////

//Joint Angles
int horizonAngle = 90;    // Range: (40,160) 
int azimuthAngle = 60;    // Range: (0,180)   axis not really usable
int lowerAngle = 100;     // Range: (77,148)
int upperAngle = 100;     // Range: (75,156)
int endAngle = 120;        // Range: (35,120)

//Specific Speeds out of 4095
//Negative number changes direction
int spinSpeed = 0; // 1024 is good
int rollSpeed = 0; // 4095 is good

// General motor speed and linear actuator speed
int speed = 2048;   // out of 4095

////////////////////////////////////////////////////////////////////////////////////
               


//Variables for angle read from potentiometers
int lowerReadAngle = 0;
int upperReadAngle = 0;


// Setup function for pins, serial for debug and PWM
void setup()
{
  //  setup serial
  Serial.begin(9600);             

  //Set pins
  pinMode(directionLowerPin, OUTPUT);
  pinMode(directionUpperPin, OUTPUT);
  pinMode(directionSpinPin, OUTPUT);
  pinMode(directionRollPin, OUTPUT);

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  yield();

  //Set all PWM's Initially to 0
  for (int i = 0; i<15; i++)
  {
    pwm.setPWM(i, 0, 0);
  }
}

// Function to decode motor direction, returns 1 if positive number, 0 otherwise
static inline int8_t isPositive(int val) {
 if (val <= 0) return 0;
 return 1;
}

//Function sets pulse width for servo if direct PWM instruction not wanted
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= 60;   // 60 Hz
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000;
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

// Function sets the lower linear actuator position given potentiometer voltage for this actuator's range of movement
void actuatorLower(int angle, int speed)
{
  
  lowerReadAngle = map(analogRead(lowerPin),291,555,148,77);        // Read PWM voltage and scale to joint angle
  
  while(lowerReadAngle != angle)                                    // While the read angle isn't the wanted angle
  {
    if (lowerReadAngle > angle)                                     // Extend or retract the actuator accordingly
    {
      pwm.setPWM(lowerPWMPin, 0, speed);
      digitalWrite(directionLowerPin, HIGH);
    }
    else if(lowerReadAngle < angle)
    {
      pwm.setPWM(lowerPWMPin, 0, speed);
      digitalWrite(directionLowerPin, LOW);
    }
   
    //Serial.println("Actuator Lower");
    //Serial.println(val1);
    lowerReadAngle = map(analogRead(lowerPin),291,555,148,77);
  }
  pwm.setPWM(lowerPWMPin, 0, 0);                                    // Stop movement
  
}

// Function sets the upper linear actuator position given potentiometer voltage for this actuator's range of movement
void actuatorUpper(int angle, int speed)
{
  upperReadAngle = map(analogRead(upperPin),95,406,156,75);              // Read PWM voltage and scale to joint angle
  
  while(upperReadAngle != angle)                                         // While the read angle isn't the wanted angle
  {

    if (upperReadAngle > angle)                                           // Extend or retract the actuator accordingly
    {
      pwm.setPWM(upperPWMPin, 0, speed);
      digitalWrite(directionUpperPin, HIGH);
    }
    else if(upperReadAngle < angle)
    {
      pwm.setPWM(upperPWMPin, 0, speed);
      digitalWrite(directionUpperPin, LOW);
    }
    
    //Serial.println("Actuator Upper");
    //Serial.println(upperReadAngle);
    //Serial.println(angle);
    upperReadAngle = map(analogRead(upperPin),95,406,156,75);
  }
  pwm.setPWM(upperPWMPin, 0, 0);                                    // Stop movement
}


//////////////////////////////////// Main Software Loop /////////////////////////////////////
void loop()
{
  actuatorLower(lowerAngle,speed);                                          // Set lower actuator position
  
  actuatorUpper(upperAngle,speed);                                          // Set upper actuator position                                

  pwm.setPWM(azimuthPWMPin, 0, map(azimuthAngle,0,180,SERVOMIN,SERVOMAX));  // Set servo position
  delay(15);                                                                // ensure the servo has time to move
  
  pwm.setPWM(horizonPWMPin, 0, map(horizonAngle,0,180,SERVOMIN,SERVOMAX));  // Set servo position
  delay(15);                                                                // ensure the servo has time to move

  pwm.setPWM(endservoPWMPin, 0, map(endAngle,0,180,SERVOMIN,SERVOMAX));     // Set servo position
  delay(15);                                                                // ensure the servo has time to move


  pwm.setPWM(spinPWMPin, 0, abs(spinSpeed));                                // Set Motor PWM
  digitalWrite(directionSpinPin, isPositive(spinSpeed));                    // Set Motor direction

  pwm.setPWM(rollPWMPin,0,abs(rollSpeed));                                  // Set Motor PWM
  digitalWrite(directionRollPin,isPositive(rollSpeed));                     // Set Motor direction

  
  
  

  
}
