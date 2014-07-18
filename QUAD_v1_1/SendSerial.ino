/*
void sendSerial()
{
  // display tab-separated accel/gyro x/y/z values
  byte lsb = 0;
  byte msb = 0;
#ifdef USE_WRITE    
  Serial.write(0x00); // start byte
  //Serial.write(0xFF);

  int kalTemp = int(kalAngleX * 10.0);
  lsb = kalTemp & 0xFF;
  msb = (kalTemp >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);

  kalTemp = int(kalAngleY * 10.0);
  lsb = kalTemp & 0xFF;
  msb = (kalTemp >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);

  lsb = dutyTime & 0xFF;
  msb = (dutyTime >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);

  lsb = battLevel & 0xFF;
  msb = (battLevel >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);

#ifdef USE_MAGNETO
  lsb = mx & 0xFF;
  msb = (mx >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);

  lsb = my & 0xFF;
  msb = (my >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);

  lsb = mz & 0xFF;
  msb = (mz >> 8) & 0xFF;
  Serial.write(lsb);
  Serial.write(msb);
#endif

  Serial.write(0x7F); //stop byte

#endif
}
*/

#include "SerialGlobal.h"

byte txBuffer[NumDataTxFields*4+4];


void sendSerial()
{
  dataTx[DataTxNumValidPackets] = numValidPackets;
  dataTx[DataTxNumFailedPackets] = numFailedPackets;
  
  
  //Calc checksum on the fly
  byte checksum = 0; 
  int packetPos = 0;
  
  txBuffer[packetPos++] = STX_BYTE; //Start byte
  //txBuffer[packetPos] = NumDataTxFields*2; //Payload lenght, manually make sure this is not a control byte
  //checksum += txBuffer[packetPos++]; 

  for (int i = 0; i < NumDataTxFields; i++)
  {
    //High byte
    txBuffer[packetPos] = (byte)((dataTx[i] >> 8) & 0x00FF);
    checksum += txBuffer[packetPos]; //Checksum calced before escaping occurs..
    //Insert escape-pattern if value happens to be a control byte
    escapeIfNeededAndIncrement(txBuffer, &packetPos);
    
    //Low byte
    txBuffer[packetPos] = (byte)(dataTx[i] & 0x00FF);
    checksum += txBuffer[packetPos];
    escapeIfNeededAndIncrement(txBuffer, &packetPos);
  }
  //Insert checksum
  txBuffer[packetPos] = checksum;  
  escapeIfNeededAndIncrement(txBuffer, &packetPos);
  
  txBuffer[packetPos++] = ETX_BYTE; //End byte
  int bytesSent = Serial.write(txBuffer, packetPos);
  if (bytesSent != packetPos)
  {
    //Serial.println("Packet Failed");
    numFailedPackets++;
  }
}

void escapeIfNeededAndIncrement(byte *buffer, int *pos)
{
  int packetPos = *pos;
  if (buffer[packetPos] == STX_BYTE ||
      buffer[packetPos] == ESC_BYTE ||
      buffer[packetPos] == ETX_BYTE)
  {
    byte b = buffer[packetPos];
    buffer[packetPos++] = ESC_BYTE;
    buffer[packetPos++] = b ^ 0xFF;
  }
  else {
    //Serial.println("POS++");
    packetPos++; //else, just increment
  }
  *pos = packetPos;
}

