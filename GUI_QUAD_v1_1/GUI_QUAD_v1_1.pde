import processing.serial.*;

import procontroll.*;
import java.io.*;

int RX_BUFFER_SIZE = 1000;
byte STX_BYTE = (byte)0xAA;
byte ESC_BYTE = (byte)0xBB;
byte ETX_BYTE = (byte)0xCC;

int SERIAL_PORT_NO = 2;

int numValidPackets = 0;
int numFailedPackets = 0;

int DataRxAngleX = 0,
  DataRxAngleY = 1,
  DataRxBatteryLevel = 2,
  DataRxDutyTime = 3,
  DataRxNumValidPackets = 4,
  DataRxNumFailedPackets = 5,
  NumDataRxFields = 6;
  
int DataTxControlThrust = 0,
  DataTxControlYaw = 1,
  DataTxControlPitch = 2,
  DataTxControlRoll = 3,
  NumDataTxFields = 4;

int[] dataRx = new int[NumDataRxFields];

int[] dataTx = new int[NumDataTxFields];



ControllIO controll;
ControllDevice device;
ControllStick leftStick;
ControllStick rightStick;
ControllButton button0;

Serial myPort;

PImage bkg;
float bkgPosX;
float bkgPosY;


float totalX = width/2;
float totalY = height/2;
float totalZ = width/2;
float totalZR = height/2;
char xAxis;

boolean enableRead = false;

float[] cpuGraphArray;
float[] battGraphArray;

float xAngle, yAngle, ZAngle;
float prevXAngle = 1, prevYAngle = 1, prevZAngle = 1;

int timePassed = 0;
final int CPU_GRAPH_VAL = 200;
final int BATT_GRAPH_VAL = 200;

//int receivedValidPackets = 0, localFailedPackets = 0;

void setup()
{
  //byteArr = new byte[NumBytesToRead];
  //DOF9Array = new int[NumBytesToRead / 2];
  
  cpuGraphArray = new float[CPU_GRAPH_VAL];
  for (int i = 0; i > CPU_GRAPH_VAL; i++)
  {
    cpuGraphArray[i] = 0;
  }
  
  battGraphArray = new float[BATT_GRAPH_VAL];
  for (int i = 0; i > BATT_GRAPH_VAL; i++)
  {
    battGraphArray[i] = 0;
  }
  
  size(1280,720);
  
  bkg = loadImage("horizon.jpg");
  //bkg.resize(width,0);
  bkgPosX = 0;//width/2;
  bkgPosY = 0;//height/2;
  
  //image(bkg, bkgPosX, bkgPosY);
  
  
  controll = ControllIO.getInstance(this);

  //device = controll.getDevice(8);
  device = controll.getDevice("Trust Gamepad");
  device.printSticks();
  device.setTolerance(0.05f);
  
  ControllSlider sliderX = device.getSlider("X Axis");
  ControllSlider sliderY = device.getSlider("Y Axis");
  ControllSlider sliderZ = device.getSlider("Z Axis");
  ControllSlider sliderZR = device.getSlider("Z Rotation");
  
  leftStick = new ControllStick(sliderX,sliderY);
  rightStick = new ControllStick(sliderZ,sliderZR);
  
  button0 = device.getButton("Button 0");
  
  //fill(0);
  rectMode(CENTER);
  
  String portName = Serial.list()[SERIAL_PORT_NO];
  myPort = new Serial(this, portName, 38400);
  
  frameRate(100);
  
}

void draw()
{
  readSerial();
  
  if(button0.pressed())
  {
    fill(255,0,0);
    //myPort.write('H');
  }else
  {
    fill(0);
    //myPort.write('L');
  }
  
  totalX = constrain(totalX + leftStick.getX() * 5.0,10,width-10);
  
  int thrust = (int)map(leftStick.getY(), -1.0, 0.0, 1000, 0);
  thrust = max(thrust, 0);
  dataTx[DataTxControlThrust] = thrust;
  
  int yaw = (int)map(leftStick.getX(), -1.0, 1.0, 500, -500);
  dataTx[DataTxControlYaw] = yaw;
  
  int pitch = (int)map(rightStick.getY(), -1.0, 1.0, 500, -500);
  dataTx[DataTxControlPitch] = pitch;
  int roll = (int)map(rightStick.getX(), -1.0, 1.0, 500, -500);
  dataTx[DataTxControlRoll] = roll;
  
  totalY = constrain(totalY + leftStick.getY() * 5.0,10,height-10);
  totalZ = constrain(totalZ + rightStick.getX() * 5.0,10,width-10);
  totalZR = constrain(totalZR + rightStick.getY() * 5.0,10,height-10);
  
  sendSerial();
  
  drawHud();
  
  
}
