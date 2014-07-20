#include <Servo.h>

#include "SerialGlobal.h"

#include <MsTimer2.h>

#include <avr/wdt.h>

#define INBUFFER_SIZE 15 //serial inbuffer
#define STATUS_LED 13
#define BATTERY_LEVEL_PIN 0
#define USE_WRITE 1 //comment to disable serial com from copter
#define SIGNAL_ERROR_ALLOWANCE 50 //maximum number of errors on signal
#define SLOW_LOOP_RATE 100 //How often the slow update loop should run
#define MAX_MOTOR_MS 2000

#define PID_DT 10.0

const byte PIDControllerPitch = 0,
           PIDControllerYaw = 1,
           PIDControllerRoll = 2,
           PIDControllerAltitude = 3,
           NumPIDControllerStates = 4;

float pidControllerErrorSum[NumPIDControllerStates];
float pidControllerLastError[NumPIDControllerStates];

byte slowLoopCounter = 0;
byte signalLostCounter = 0;

int dutytimeCounter = 0;
unsigned long timeCounter = 0;

int fr_motor_speed, br_motor_speed, bl_motor_speed, fl_motor_speed;

Servo rightMotor, rearMotor, leftMotor, frontMotor;

void setup()
{
  Serial.begin(38400);

  pinMode(STATUS_LED, OUTPUT);
  pinMode(BATTERY_LEVEL_PIN, INPUT);

  rightMotor.attach(2, 1000, MAX_MOTOR_MS);
  rearMotor.attach(3, 1000, MAX_MOTOR_MS);
  leftMotor.attach(4, 1000, MAX_MOTOR_MS);
  frontMotor.attach(5, 1000, MAX_MOTOR_MS);

  fr_motor_speed = 1000;
  br_motor_speed = 1000;
  fl_motor_speed = 1000;
  bl_motor_speed = 1000;

  rightMotor.writeMicroseconds(fr_motor_speed);
  rearMotor.writeMicroseconds(br_motor_speed);
  leftMotor.writeMicroseconds(bl_motor_speed);
  frontMotor.writeMicroseconds(fl_motor_speed);

  setupmpu6050();

  MsTimer2::set(PID_DT, controlLoop); // 500ms period
  MsTimer2::start();

  memset(pidControllerErrorSum, 0, sizeof(pidControllerErrorSum));
  memset(pidControllerLastError, 0, sizeof(pidControllerLastError));

  wdt_enable (WDTO_1S);  // reset after one second, if no "pat the dog" received

  timeCounter = micros();
}

void controlLoop()
{
  float controlPitch = calculatePID(PIDControllerPitch, (float)dataRx[DataRxControlPitch] - (get_last_y_angle() * 2.7778), 1.0, 0.0, -10.0);
  //dataTx[DataTxDutyTime] = controlPitch;
  
  int thrust = 1000 + dataRx[DataRxControlThrust];
  dataTx[DataTxDutyTime] = controlPitch;
  int frontThrust = max(1000, thrust - controlPitch);
  int rearThrust = max(1000, thrust + controlPitch);

  int leftThrust = max(1000, thrust - dataRx[DataRxControlRoll]);
  int rightThrust = max(1000, thrust + dataRx[DataRxControlRoll]);

  if (1000 < thrust) //sageguard
  {
    frontMotor.writeMicroseconds(frontThrust);
    rearMotor.writeMicroseconds(rearThrust);
//  leftMotor.writeMicroseconds(leftThrust);
//  rightMotor.writeMicroseconds(rightThrust);
  }
  else
  {
    frontMotor.writeMicroseconds(1000);
    rearMotor.writeMicroseconds(1000);
//  leftMotor.writeMicroseconds(1000);
//  rightMotor.writeMicroseconds(1000);
  }
}

float calculatePID(byte controller, float error, float kp, float ki, float kd) {
  /*Compute all the working error variables*/
  pidControllerErrorSum[controller] += (error * PID_DT);
  pidControllerErrorSum[controller] = max(-500, min(pidControllerErrorSum[controller], 500));
  float dErr = (error - pidControllerLastError[controller]) / PID_DT;

  /*Remember some variables for next time*/
  pidControllerLastError[controller] = error;

  /*Compute PID Output*/
  return kp * error + ki * pidControllerErrorSum[controller] + kd * dErr;
}

void slowLoop()
{
  dataTx[DataTxBatteryLevel] = (int)analogRead(BATTERY_LEVEL_PIN);

}

void fastLoop()
{
  
  wdt_reset ();  // reset watchdog timer
  
  // read raw accel/gyro measurements from device
  if (signalLostCounter++ > SIGNAL_ERROR_ALLOWANCE)
  {
    //MAJOR ERROR!
    digitalWrite(STATUS_LED, HIGH);
  }

  readAngles();
  
  //dataTx[DataTxDutytimeCounterCounter] = thrust;// dutytimeCounterCounter;
  //dataTx[DataTxBatteryLevel] = battLevel;

}

unsigned long lastFastLooptimeCounterStamp = 0;
unsigned long lastSlowLooptimeCounterStamp = 0;

unsigned long fastLoopDuration = 0;
unsigned long slowLoopDuration = 0;

void loop()
{
  readSerial();
  timeCounter = micros();

  if (10000 < (timeCounter - lastFastLooptimeCounterStamp))
  {
    lastFastLooptimeCounterStamp = timeCounter;
    fastLoop(); // get calls every 1/100s
    fastLoopDuration = micros() - timeCounter;
  }

  if (100000 < (timeCounter - lastSlowLooptimeCounterStamp))
  {
    lastSlowLooptimeCounterStamp = timeCounter;
    slowLoop(); // get calls every 1s
    slowLoopDuration = micros() - timeCounter;

    dutytimeCounter = (fastLoopDuration / 100) + (slowLoopDuration / 1000); //TODO: beräknas det helt rätt? acc
  }


}
