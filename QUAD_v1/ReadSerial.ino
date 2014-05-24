

byte calcChecksum(byte *packet, byte payLengt)
{
    byte checksum = 0;
    byte i;
    for (i = 0; i < payLengt; i++)
    {
        checksum = checksum + packet[i];
    }
    return checksum;
}

void parsePayload()
{
  //Go through the raw packet and "un-escape" bytes
        int length = bytesRead - 1; //without start byte and stop byte
        byte packet[length];
        
          int i;
          int p = 0;
          for (i = 1; i < length; i++)
          {
           
              if (0xBB == arr[i])
              {
                  i++;
                  switch (arr[i])
                  {
                      //case 0xFF:
                      case 0x55:
                      case 0x44:
                      case 0x33:
                          packet[p] = (arr[i] ^ 0xFF);
                          p++;
                          break;
                      default:
                      {
                          //printf("Invalid escape pattern (%.4X)\n", debugPayloadID);
                          //numFailedPackets++;
                          return;
                          break;
                      }
                  }
              }
              else
              {
                  packet[p] = arr[i];
                  p++;
              }
          }
          
          //The actual packet length after un-escaping
          length = p;
          //Serial.write(p + 100);
          byte chksum = calcChecksum(packet, length - 1);
          //Serial.write(chksum);
          //Serial.write("A");
          //Serial.write(packet[length - 1]);
          if ((chksum & 0xFF) == packet[length - 1])
          {
             //Serial.write("A");
            //OK packet!
            // OK Thrust data from controller
            controlThrust = packet[0];
            //Serial.write(controlThrust);
            //controlThrust = controlThrust << 8;
            //controlThrust = controlThrust | (packet[0] & 0xFF);
            
            //clear errorcounter
            signalLostCounter = 0;
            digitalWrite(STATUS_LED, LOW);
            //Serial.write(controlThrust);
          }
          else
          {
            //Serial.write("B");
            //Bad packet
            //digitalWrite(STATUS_LED, HIGH);
          }
}

void readSerial()
{
  //read serial
  //int serAvail = Serial.available();
  
    while (Serial.available() > 0)
    {
      //Serial.write(Serial.available());
      byte inByte = Serial.read() & 0xFF;
      //Serial.write(inByte);
      if (enableRead)
      {
          arr[bytesRead] = inByte;
          bytesRead++;
          
          if (inByte == 0xCC && arr[0] == 0xAA)//)//end byte or doo much data
        {
          
          
          
          
          //Serial.write(0x01);
          //Serial.println("inbyte = 0x5A");
          //arr[bytesRead] = inByte;
          //bytesRead++;
          //Serial.write(arr[0]);
          if (bytesRead > 6)
          {
            parsePayload();
          }
          else
          {
//            for (int i = 0; i < bytesRead; i++)
//            {
//              Serial.write(arr[i]);
//            }
          }
          
          //break;
        }
      }
      
      if (inByte == 0xAA)//start byte
      {
            //enableRead = 0;
            bytesRead = 0;
            //isDone = 0;
        //Serial.write(0x00);
        //Serial.println("inbyte = 0x5A");
        arr[0] = inByte;
        bytesRead++;
        enableRead = 1;
      }
    }
}
