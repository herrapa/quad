
void doMath()
{
  float x, y, z, vectLength;
  x = dataRx[DataRxAngleX] / 16384.0;
  y = dataRx[DataRxAngleY] / 16384.0;
  z = 1; //dataRx[DataRxAngleZ] / 16384.0;
  
  vectLength = sqrt((x * x) + (y * y) + (z * z));
  x = x / vectLength;
  y = y / vectLength;
  z = z / vectLength;
  //println((acos(x) * 0.1f));
  //xAngle = ;
  //float temp = (acos(x) * 0.1f);
  //println(temp);
  
  if (prevXAngle != prevXAngle) // NaN test
    prevXAngle = 1.0;
    
    if (prevYAngle != prevYAngle) // NaN test
    prevYAngle = 1.0;
  
  prevXAngle = (prevXAngle * 0.85f) + (acos(x) * 0.15f);
  prevYAngle = (prevYAngle * 0.85f) + (acos(y) * 0.15f);
  //println(prevYAngle);
}
