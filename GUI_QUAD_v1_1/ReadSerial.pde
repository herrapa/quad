
//import SerialGlobal
boolean startByteFound = false;

byte[] rxBuffer = new byte[RX_BUFFER_SIZE];
byte[] packetBuffer = new byte[RX_BUFFER_SIZE];
int packetBufferPos = 0;
int bytesAvailable = 0;
int bytesRead = 0;

void readSerial()
{
  bytesAvailable = myPort.available();
  
  if (0 < bytesAvailable)
  {
    bytesRead = myPort.readBytes(rxBuffer);
    //println(bytesRead);
    for (int i = 0; i < bytesRead; i++)
    {
      if (STX_BYTE == rxBuffer[i]) //if start byte found, reset buffer
      {
        //println("Startbytefound");
        startByteFound = true;
        packetBufferPos = 0;
        packetBuffer[packetBufferPos++] = rxBuffer[i];
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
            //println("pkt");
            //println(packetBufferPos);
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
  
}

void parseData(byte[] rawPacket, int packetLength)
{
  int packetPos = 0;
  byte checksum = 0;
  //println("---");
  //println(packetLength);
  for (int i = 0; i < packetLength; i++)
  {
    //println(rawPacket[i]);
    if (ESC_BYTE == rawPacket[i])
    {
      i++;
      switch(rawPacket[i])
      {
        case 0x55: //STX_BYTE xor 0xFF
        case 0x44: //ESC_BYTE:
        case 0x33: //ETX_BYTE:
          rawPacket[packetPos] = (byte)(rawPacket[i] ^ 0xFF);
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
  byte receivedChecksum = rawPacket[packetPos-1]; // correct ?
  checksum -= receivedChecksum; //SUBTRACT THE received unescaped checksum
  //print("Received checksum: ");
  //println(receivedChecksum);
  //print("checksum: ");
  //println(checksum);
  if (receivedChecksum != checksum)
  {
    numFailedPackets++;
    return;
  }
  numValidPackets++;
  
  int j = 1; //0 is payload length field
  for (int i = 0; i < NumDataRxFields; i++) 
  {
    dataRx[i] = rawPacket[j] * 256 + rawPacket[j + 1] % 256;
    j+=2;
  }
  
  //dataRx byte -> int
  //receivedValidPackets = dataRx[DataTxNumValidPackets];// = numValidPackets;
  //receivedFailedPackets = dataRx[DataTxNumFailedPackets]; // = numFailedPackets;
  
}

