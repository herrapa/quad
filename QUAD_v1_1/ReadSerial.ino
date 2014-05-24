
#include "SerialGlobal.h"
boolean startByteFound = false;

byte rxBuffer[RX_BUFFER_SIZE];
byte packetBuffer[RX_BUFFER_SIZE];
int packetBufferPos = 0;

void readSerial()
{
  int bytesAvailable = Serial.available();
  
  if (0 < bytesAvailable)
  {
    int bytesRead = Serial.readBytes((char*)rxBuffer, bytesAvailable);    
    for (int i = 0; i < bytesRead; i++)
    {
      if (STX_BYTE == rxBuffer[i]) //if start byte found, reset buffer
      {
        startByteFound = true;
        packetBufferPos = 0;
        continue;
      }
      else if (startByteFound)
      {
        if (RX_BUFFER_SIZE > packetBufferPos)
        {
          if (ETX_BYTE != rxBuffer[i])
          {
            packetBuffer[packetBufferPos++] = rxBuffer[i];
          }
          else //stop byte found
          {
            parseData(packetBuffer, packetBufferPos);
            startByteFound = false;
            packetBufferPos = 0;
          }
        }
        else 
        {
          numFailedPackets++;
          startByteFound = false;
          packetBufferPos = 0;
        }
      }
    }
  }
  
  
  
  /*
    while (Serial.available() > 0)
    {
      byte inByte = Serial.read() & 0xFF;
      if (enableRead)
      {
          arr[bytesRead] = inByte;
          bytesRead++;
          
          if (inByte == 0xCC && arr[0] == 0xAA)//)//end byte or too much data
        {
          if (bytesRead > 6)
          {
            parsePayload();
          }
        }
      }
      
      if (inByte == 0xAA)//start byte
      {
        bytesRead = 0;
        arr[0] = inByte;
        bytesRead++;
        enableRead = 1;
      }
    }
    */
}

void parseData(byte* rawPacket, int length)
{
  int packetPos = 0;
  byte checksum = 0;
  for (int i = 0; i < length; i++)
  {
    if (ESC_BYTE == rawPacket[i])
    {
      i++;
      switch(rawPacket[i])
      {
        case 0x55: //STX_BYTE xor 0xFF
        case 0x44: //ESC_BYTE:
        case 0x33: //ETX_BYTE:
          rawPacket[packetPos] = rawPacket[i] ^ 0xFF;
          checksum += rawPacket[packetPos];
          packetPos++;
          break;
        default:
          numFailedPackets++;
          return;
      }  
    }
    else
    {
      rawPacket[packetPos] = rawPacket[i];
      checksum += rawPacket[packetPos];
      packetPos++;
    }
  }
  byte receivedChecksum = rawPacket[packetPos-1];
  checksum -= receivedChecksum; //SUBTRACT THE received unescaped checksum
  
  if (receivedChecksum != checksum)
  {
    numFailedPackets++;
    return;
  }
  numValidPackets++;
  //TODO: data -> array
}




