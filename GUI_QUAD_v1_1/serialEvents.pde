
/*
void checkSerial()
{
  //println("----------");
  while ( myPort.available() > 0) 
  {  
    int inByte = myPort.read() & 0xFF;
    //print("r: ");
      //println((byte)inByte);
      if (isDone)
      {
        if (inByte == 0x7F)
          {
            //println("= 7F");
            parseSerialPacket();
            enableRead = false;
            bytesRead = 0;
            isDone = false;
          }
        else
          {
            //bad data
            //println("Bad data");
            //println(inByte);
            enableRead = false;
            bytesRead = 0;
            isDone = false;
          }
      }
          
      if (enableRead)
      {
        //println("enableRead");
        byteArr[bytesRead] = (byte)inByte;
        
        if (bytesRead == (NumBytesToRead - 1))
        {
          isDone = true;
        }
        
        bytesRead++;
      }
      
      if (inByte == 0x00)
      {
        //println("inbyte = 0x00");
        enableRead = true;
      }
  }
}

void parseSerialPacket()
{
  println("parsing: ");
  for (int i = 0; i < NumBytesToRead / 2; i++)
  {
    short data = 0;
    data = (short)(0x00FF & byteArr[i * 2 + 1]);
    data = (short)(data << 8);
    data = (short)(data | (byteArr[i * 2] & 0xFF));
    
    DOF9Array[i] = (int)data;
    println(data);
    
    
    //println("data" + i + ": "  + DOF9Array[i]);
  }
  //doMath();
}

byte calcChecksum(byte[] packet, byte payLengt)
{
    byte checksum = 0;
    byte i;
    for (i = 0; i < payLengt; i++)
    {
        checksum = (byte)(checksum + packet[i]);
    }
    return checksum;
}

void sendJoystickSerial (float thrust, float yaw, float pitch, float roll)
{
  byte thrust_byte = (byte)(thrust * 127);//map(thrust, -1.0, 1.0, -127, 127);
  byte yaw_byte = (byte)(yaw * 127);//map(yaw, -1.0, 1.0, -127, 127);
  byte pitch_byte = (byte)(pitch * 127);//map(pitch, -1.0, 1.0, -127, 127);
  byte roll_byte = (byte)(roll * 127);//map(roll, -1.0, 1.0, -127, 127);
  
  byte[] payLoad = new byte[7];
  byte payLoadLen = 0;
  //byte tmpPacket;
  
  payLoad[payLoadLen] = thrust_byte;
  payLoadLen++;
  
  payLoad[payLoadLen] = yaw_byte;
  payLoadLen++;
  
  payLoad[payLoadLen] = pitch_byte;
  payLoadLen++;
  
  payLoad[payLoadLen] = roll_byte;
  payLoadLen++;
  
  byte checkSum = calcChecksum(payLoad, payLoadLen);
  //println(checkSum);
  payLoad[payLoadLen] = checkSum;
  payLoadLen++; 
  
  byte[] escapedPayLoad = new byte[15];
  byte escapedPayLoadLen = 0;
  
  escapedPayLoad[escapedPayLoadLen] = QUAD_STX; // add start packet
  escapedPayLoadLen++;
  
  for (int i = 0; i < payLoadLen; i++)
  {
      switch ((int)payLoad[i] & 0xFF)
    {
      case 0xAA:
      case 0xBB:
      case 0xCC:
      {
        println("pkt escaped");
        escapedPayLoad[escapedPayLoadLen] = QUAD_ESC; // add escape packet
        escapedPayLoadLen++;
        escapedPayLoad[escapedPayLoadLen] = (byte)(payLoad[i] ^ 0xFF); // add XORed packet
        escapedPayLoadLen++;
      }
      default:
      {
        escapedPayLoad[escapedPayLoadLen] = payLoad[i]; // add packet to payload directly
        escapedPayLoadLen++;
      }
    }
  }
  
  escapedPayLoad[escapedPayLoadLen] = QUAD_ETX;
  escapedPayLoadLen++;
  //println(payLoadLen);
  
  //myPort.write(payLoad);
  
  for(int i = 0; i < escapedPayLoadLen; i++)
  {
    //print("s: ");
    //println(escapedPayLoad[i]);
    myPort.write(escapedPayLoad[i]);
    //delay(5);
  } 
}
*/
