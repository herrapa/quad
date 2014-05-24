
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Servo.h> 

#define INBUFFER_SIZE 15 //serial inbuffer
#define STATUS_LED 13
#define USE_WRITE 1 //comment to disable serial com from copter
#define SIGNAL_ERROR_ALLOWANCE 50 //maximum number of errors on signal
#define SLOW_LOOP_RATE 100 //How often the slow update loop should run
#define MAX_FAN_MS 1200


MPU6050 accelgyro; //with default adress, 0x68

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

byte controlThrust = 0;
byte controlYaw = 0;
byte controlPitch = 0;
byte controlRoll = 0;

unsigned long time = 0;

byte enableRead = 0;
byte bytesRead = 0;
byte isDone = 0;
byte arr[INBUFFER_SIZE]; //control data array
byte slowLoopCounter = 0;
byte signalLostCounter = 0;

Servo frontRightMotor, backRightMotor, backLeftMotor, frontLeftMotor;

void setup() 
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Serial.begin(38400);
    
    pinMode(STATUS_LED, OUTPUT);
    
    frontRightMotor.attach(2, 1000, MAX_FAN_MS);
    backRightMotor.attach(3, 1000, MAX_FAN_MS);
    backLeftMotor.attach(4, 1000, MAX_FAN_MS);
    frontLeftMotor.attach(5, 1000, MAX_FAN_MS);
    
    frontRightMotor.writeMicroseconds(1000);
    backRightMotor.writeMicroseconds(1000);
    backLeftMotor.writeMicroseconds(1000);
    frontLeftMotor.writeMicroseconds(1000);
    
    accelgyro.initialize();
}

void slowLoop()
{
  //check battery
  //check magnetic heading?
  //accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
}

void fastLoop()
{
  // read raw accel/gyro measurements from device
    
    
    readSerial();
    
    if (signalLostCounter++ > SIGNAL_ERROR_ALLOWANCE)
    {
      //MAJOR ERROR!
      digitalWrite(STATUS_LED, HIGH);
    }
    
    // these methods (and a few others) are also available
    accelgyro.getAcceleration(&ax, &ay, &az);
    accelgyro.getRotation(&gx, &gy, &gz);
    
    //frontRightMotor.writeMicroseconds(map(controlThrust, 0, 65535, 1000, 2000));
    int thrust = 1000 + abs((controlThrust << 8)) / 32;
    backRightMotor.writeMicroseconds(thrust);
    frontRightMotor.writeMicroseconds(thrust);
    backLeftMotor.writeMicroseconds(thrust);
    frontLeftMotor.writeMicroseconds(thrust);
   
    sendSerial();
}

void loop() 
{
  time = micros();
    
    fastLoop();
    
    if (slowLoopCounter++ > SLOW_LOOP_RATE)
    {
      slowLoopCounter = 0;
      slowLoop();
    }
    
    time = micros() - time;
    delayMicroseconds(10000 - time);
}
