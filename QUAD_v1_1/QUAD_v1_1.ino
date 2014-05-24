
// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "Wire.h"
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Kalman.h"
#include <Servo.h> 

#include "SerialGlobal.h"

#define INBUFFER_SIZE 15 //serial inbuffer
#define STATUS_LED 13
#define BATTERY_LEVEL_PIN 0
#define USE_WRITE 1 //comment to disable serial com from copter
#define SIGNAL_ERROR_ALLOWANCE 50 //maximum number of errors on signal
#define SLOW_LOOP_RATE 100 //How often the slow update loop should run
#define MAX_MOTOR_MS 2000


MPU6050 accelgyro; //with default adress, 0x68

Kalman kalmanX; // Create the Kalman instances
Kalman kalmanY;

int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;

double accXangle, accYangle; // Angle calculate using the accelerometer
double gyroXangle, gyroYangle; // Angle calculate using the gyro
//double kalAngleX, kalAngleY; // Calculate the angle using a Kalman filter

//byte controlThrust = 0;
//byte controlYaw = 0;
//byte controlPitch = 0;
//byte controlRoll = 0;



//byte enableRead = 0;
//byte bytesRead = 0;
//byte isDone = 0;
//byte arr[INBUFFER_SIZE]; //control data array

byte slowLoopCounter = 0;
byte signalLostCounter = 0;

int dutyTime = 0;
//int battLevel;

unsigned long time = 0;

int fr_motor_speed, br_motor_speed, bl_motor_speed, fl_motor_speed;

Servo frontRightMotor, backRightMotor, backLeftMotor, frontLeftMotor;

void setup() 
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Serial.begin(38400);
    
    pinMode(STATUS_LED, OUTPUT);
    pinMode(BATTERY_LEVEL_PIN, INPUT);
    
    frontRightMotor.attach(2, 1000, MAX_MOTOR_MS);
    backRightMotor.attach(3, 1000, MAX_MOTOR_MS);
    backLeftMotor.attach(4, 1000, MAX_MOTOR_MS);
    frontLeftMotor.attach(5, 1000, MAX_MOTOR_MS);
    
    fr_motor_speed = 1000;
    br_motor_speed = 1000;
    fl_motor_speed = 1000;
    bl_motor_speed = 1000;
    
    frontRightMotor.writeMicroseconds(fr_motor_speed);
    backRightMotor.writeMicroseconds(br_motor_speed);
    backLeftMotor.writeMicroseconds(bl_motor_speed);
    frontLeftMotor.writeMicroseconds(fl_motor_speed);
    
    accelgyro.initialize();
    delay(100); // Wait for sensor to stabilize
    
    accelgyro.getAcceleration(&ax, &ay, &az);
    
    accYangle = (atan2(ax,az)+PI)*RAD_TO_DEG;
    accXangle = (atan2(ay,az)+PI)*RAD_TO_DEG;
    
    kalmanX.setAngle(accXangle); // Set starting angle
    kalmanY.setAngle(accYangle);
    gyroXangle = accXangle;
    gyroYangle = accYangle;
    
    time = micros();
}

void slowLoop()
{
  dataTx[DataTxBatteryLevel] = (int)analogRead(BATTERY_LEVEL_PIN);
  //delay(100);
  //TODO:
  //check battery
  //duty cycle %
  //check magnetic heading?
  //accelgyro.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
}

void fastLoop()
{
  // read raw accel/gyro measurements from device
    
    
    if (signalLostCounter++ > SIGNAL_ERROR_ALLOWANCE)
    {
      //MAJOR ERROR!
      digitalWrite(STATUS_LED, HIGH);
    }
    
    calcAngles();
    
    
    
    //int thrust = 1000 + abs((controlThrust << 8)) / 32; //controlThrust is from -127 to 127
    //backRightMotor.writeMicroseconds(thrust);
    //frontRightMotor.writeMicroseconds(thrust);
    //backLeftMotor.writeMicroseconds(thrust);
    //frontLeftMotor.writeMicroseconds(thrust);
   
   //dataTx[DataTxAngleX] = int(kalAngleX * 100);
   //dataTx[DataTxAngleY] = int(kalAngleY * 100);
   dataTx[DataTxDutyTime] = dutyTime;
   //dataTx[DataTxBatteryLevel] = battLevel;
   
   //dataTx[DataTxNumValidPackets] = numValidPackets;
   //dataTx[DataTxNumFailedPackets] = numFailedPackets;
   
    sendSerial();
}

unsigned long lastFastLoopTimeStamp = 0;
unsigned long lastSlowLoopTimeStamp = 0;

int fastLoopDuration = 0;
int slowLoopDuration = 0;

void loop() 
{
  time = micros();
  
  readSerial();
  
  if (10000 < (time - lastFastLoopTimeStamp)) 
  {
    lastFastLoopTimeStamp = time;
    fastLoop(); // gets calles every 1/100s
    fastLoopDuration = micros() - time;
  }
  
  if (1000000 < (time - lastSlowLoopTimeStamp)) 
  {
    lastSlowLoopTimeStamp = time;
    slowLoop(); // gets calles every 1s
    slowLoopDuration = micros() - time;
    
    dutyTime = (fastLoopDuration / 100) + (slowLoopDuration / 10000); //TODO: beräknas det helt rätt? acc
  }
  
  
}
