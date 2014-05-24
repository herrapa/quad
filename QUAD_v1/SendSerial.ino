
void sendSerial()
{
      // display tab-separated accel/gyro x/y/z values
    byte lsb = 0;
    byte msb = 0;
#ifdef USE_WRITE    
    Serial.write(0x00); // start byte
    //Serial.write(0xFF);
 
    lsb = ax & 0xFF;
    msb = (ax >> 8) & 0xFF;
    Serial.write(lsb);
    Serial.write(msb);
    
    lsb = ay & 0xFF;
    msb = (ay >> 8) & 0xFF;
    Serial.write(lsb);
    Serial.write(msb);
    
    lsb = az & 0xFF;
    msb = (az >> 8) & 0xFF;
    Serial.write(lsb);
    Serial.write(msb);
    
    lsb = gx & 0xFF;
    msb = (gx >> 8) & 0xFF;
    Serial.write(lsb);
    Serial.write(msb);
    
    lsb = gy & 0xFF;
    msb = (gy >> 8) & 0xFF;
    Serial.write(lsb);
    Serial.write(msb);
    
    lsb = gz & 0xFF;
    msb = (gz >> 8) & 0xFF;
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
