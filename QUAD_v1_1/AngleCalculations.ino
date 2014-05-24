unsigned long gyroTime = 0;
int gyroDutyTime = 0;
void calcAngles()
{
  // these methods (and a few others) are also available
    accelgyro.getAcceleration(&ax, &ay, &az);
    accelgyro.getRotation(&gx, &gy, &gz);
    
    accYangle = (atan2(ax,az)+PI)*RAD_TO_DEG;
    accXangle = (atan2(ay,az)+PI)*RAD_TO_DEG;
    
    git
    gyroDutyTime = micros() - gyroDutyTime;
    gyroTime = micros();
    
  gyroXangle += gyroXrate*(gyroDutyTime/1000000); // Calculate gyro angle without any filter  
  gyroYangle += gyroYrate*(gyroDutyTime/1000000);
  
  dataTx[DataTxAngleX] = (int)(accYangle * 91.0);//(int)(100.0 * kalmanX.getAngle(accXangle, gyroXrate, (double)gyroDutyTime/1000000)); // Calculate the angle using a Kalman filter
  dataTx[DataTxAngleY] = (int)(accXangle * 91.0);//(int)(100.0 * kalmanY.getAngle(accYangle, gyroYrate, (double)gyroDutyTime/1000000));
  //TODO:
  //skicka in kalAngle till processing, använda till balancering
  
  
}
