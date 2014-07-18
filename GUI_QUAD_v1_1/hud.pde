//code for drawing a Heads Up Display



float hudOffsetX = 250.0;
float hudOffsetY = 100.0;
float hudInfoRectWidth = 100.0;
float hudInfoRectHeight = 30.0;

float hudCrossSize = 30.0;

float hudHorizonLineSize = 200.0;

float hudCircleSize = hudHorizonLineSize * 2.0 + 20.0;

void drawHud ()
{
  //println(dataRx[DataRxAngleX] / 91.0f);
  float thrust = leftStick.getY();
  float yaw = leftStick.getX();
  float pitch = (((float)dataRx[DataRxAngleX]) * 0.00019179442f);// - PI;//prevXAngle;
  float roll = ((float)dataRx[DataRxAngleY]) * 0.00019179442f;/// 91.0f * DEG_TO_RAD;//prevYAngle;
  //println(pitch);
  float centerX = width/2;
  float centerY = height/2;
  
  float dutyCycle = (float)dataRx[DataRxDutyTime]; // / 10000 * 100
  
  //cpu level graph array:
  for (int i = 0; i < CPU_GRAPH_VAL - 1; i++)
  {
    cpuGraphArray[i] = cpuGraphArray[i + 1];
  }
  cpuGraphArray[CPU_GRAPH_VAL - 1] = dutyCycle;
  
  float battLevel = dataRx[DataRxBatteryLevel] * 0.5;//0.01434; //Need calculations
  
  //battery level graph array:
  for (int i = 0; i < BATT_GRAPH_VAL - 1; i++)
  {
    battGraphArray[i] = battGraphArray[i + 1];
  }
  battGraphArray[BATT_GRAPH_VAL - 1] = battLevel;
  
  imageMode(CENTER);

  image(bkg, centerX, centerY, width, height); //background

  int thrustInt = (int)(-thrust * 100.0);

  bkgPosY = (pitch) * 200.0f;
  bkgPosX = yaw * 200.0f;

  pushMatrix();

  translate(centerX + bkgPosX, centerY - bkgPosY);

  noFill();
  ellipseMode(CENTER);
  rectMode(CENTER);
  ellipse(0, 0, hudCircleSize, hudCircleSize);

  for (int j = 0; j <= 1; j++)
  {
    for (int i = 0; i <= 9; i++)
    {
      String s = "" + i * 10;
      float x = hudHorizonLineSize * cos(radians(i * 10 + 180 * j));
      float y = hudHorizonLineSize * sin(radians(i * 10 + 180 * j));
      line(x * 1.05, y * 1.05, x * 1.1, y * 1.1);
      text(s, x * 1.15, y * 1.15);
    }
  }

  for (int j = 0; j <= 1; j++)
  {
    for (int i = 0; i < 9; i++)
    {
      String s = "" + i * 10;
      float x = hudHorizonLineSize * cos(radians(-i * 10 + 180 * j));
      float y = hudHorizonLineSize * sin(radians(-i * 10 + 180 * j));
      line(x * 1.05, y * 1.05, x * 1.1, y * 1.1);
      text(s, x * 1.15, y * 1.15);
      text(s, x * 1.15, y * 1.15);
    }
  }
  
  rotate(roll);

  strokeWeight(3);
  for (int i = -3; i <= 3; i++)
  {
    line( - (hudHorizonLineSize - abs(i * 50)), - i * 60.0, 0 + (hudHorizonLineSize - abs(i * 50)), - i * 60.0);
  }

  popMatrix();

  //rotate(0);
  text("CPU: ", 30, 20);
  String st = "" + dutyCycle;
  text(st, 80, 20);
  
  //CPU graph
  line(0, 180,CPU_GRAPH_VAL, 180);
  line(0, 80,CPU_GRAPH_VAL, 80);
  for (int i = 0; i < CPU_GRAPH_VAL - 1; i++) 
  {
    stroke(0);
    strokeWeight(1);
    line(i, 180 - cpuGraphArray[i],i+1, 180 - cpuGraphArray[i+1]);
  }
  
  text("Batt: ", 30, 225);
  st = "" + battLevel;
  text(st, 100, 225);
  
  //Battery graph
  stroke(0, 255, 0, 200);
  strokeWeight(3);
  line(0, 350,BATT_GRAPH_VAL, 350);
  line(0, 250,BATT_GRAPH_VAL, 250);
  for (int i = 0; i < BATT_GRAPH_VAL - 1; i++) 
  {
    stroke(0);
    strokeWeight(1);
    line(i, 350 - (battGraphArray[i] * 6.8),i+1, 350 - (battGraphArray[i+1] * 6.8));
  }
  
  text("Valid: ", 35, 425);
  st = "" + numValidPackets;
  text(st, 100, 425);
  text("Failed: ", 35, 455);
  st = "" + numFailedPackets;
  text(st, 100, 455);
  text("rec. Valid: ", 35, 485);
  st = "" + dataRx[DataRxNumValidPackets];
  text(st, 100, 485);
  text("rec. Failed: ", 35, 515);
  st = "" + dataRx[DataRxNumFailedPackets];
  text(st, 100, 515);
  
  text("X: ", 35, 545);
  st = "" + pitch;
  text(st, 100, 545);
  text("Y: ", 35, 575);
  st = "" + roll;
  text(st, 100, 575);



  //cross
  stroke(0, 255, 0, 200);
  strokeWeight(3);
  line(centerX - hudCrossSize, centerY, centerX + hudCrossSize, centerY);
  line(centerX, centerY - hudCrossSize, centerX, centerY + hudCrossSize);

  //Thrust and altitude info rects
  rectMode(CENTER);
  noFill();
  strokeWeight(2);
  rect(centerX - hudOffsetX, centerY - hudOffsetY, hudInfoRectWidth, hudInfoRectHeight); //left rect
  rect(centerX - hudOffsetX, centerY - hudOffsetY - hudInfoRectHeight, hudInfoRectWidth, hudInfoRectHeight);


  rect(centerX + hudOffsetX, centerY - hudOffsetY, hudInfoRectWidth, hudInfoRectHeight); //right rect
  rect(centerX + hudOffsetX, centerY - hudOffsetY - hudInfoRectHeight, hudInfoRectWidth, hudInfoRectHeight);

  fill(0, 255, 0, 200);
  textAlign(CENTER, CENTER);
  textSize(20);
  text("Thrust", centerX - hudOffsetX, centerY - hudOffsetY - hudInfoRectHeight, hudInfoRectWidth, hudInfoRectHeight);
  text("Alt", centerX + hudOffsetX, centerY - hudOffsetY - hudInfoRectHeight, hudInfoRectWidth, hudInfoRectHeight);
  String s = ""+thrustInt;
  text(s, centerX - hudOffsetX, centerY - hudOffsetY, hudInfoRectWidth, hudInfoRectHeight);
  text("200", centerX + hudOffsetX, centerY - hudOffsetY, hudInfoRectWidth, hudInfoRectHeight);
}

