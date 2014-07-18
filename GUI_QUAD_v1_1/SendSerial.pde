
byte[] txBuffer = new byte[NumDataTxFields * 4 + 4];

void sendSerial()
{
  //Calc checksum on the fly
  byte checksum = 0; 
  int packetPos = 0;
  
  txBuffer[packetPos++] = STX_BYTE; //Start byte
  
  for (int i = 0; i < NumDataTxFields; i++)
  {
    //High byte
    short value = (short)dataTx[i]; //typecast 32-bit int to 16-bit short to preserve sign-bit
    txBuffer[packetPos] = (byte)((value >> 8) & 0x00FF);
    checksum += txBuffer[packetPos]; //Checksum calced before escaping occurs..
    //Insert escape-pattern if value happens to be a control byte
    packetPos = escapeIfNeededAndIncrement(txBuffer, packetPos);
    
    //Low byte
    txBuffer[packetPos] = (byte)(value & 0x00FF);
    checksum += txBuffer[packetPos];
    packetPos = escapeIfNeededAndIncrement(txBuffer, packetPos);
  }

  //Insert checksum
  txBuffer[packetPos] = checksum;  
  packetPos = escapeIfNeededAndIncrement(txBuffer, packetPos);
  
  txBuffer[packetPos++] = ETX_BYTE; //End byte

  myPort.write(txBuffer);
}

int escapeIfNeededAndIncrement(byte[] buffer, int pos)
{
  if (buffer[pos] == STX_BYTE || buffer[pos] == ESC_BYTE || buffer[pos] == ETX_BYTE)
  {
    byte b = buffer[pos];
    buffer[pos++] = ESC_BYTE;
    buffer[pos++] = (byte)(b ^ 0xFF);
  }
  else {
    pos++; //else, just increment
  }
  return pos;
}

