package com.quad;

/**
 * Created by Jocke on 2014-07-18.
 */

public class SerialEventManager {

    private Serial myPort;

    private int RX_BUFFER_SIZE = 1000;
    private byte STX_BYTE = (byte)0xAA;
    private byte ESC_BYTE = (byte)0xBB;
    private byte ETX_BYTE = (byte)0xCC;

    private int SERIAL_PORT_NO = 2;

    private int numValidPackets = 0;
    private int numFailedPackets = 0;

    private short[] dataRx;
    private short[] dataTx;

    private boolean startByteFound = false;

    private byte[] rxBuffer;
    private byte[] packetBuffer;
    private int packetBufferPos = 0;
    private int bytesAvailable = 0;
    private int bytesRead = 0;

    private byte[] txBuffer;

    public enum DataRxEnum {
        DataRxAngleX (0),
        DataRxAngleY(1),
        DataRxBatteryLevel(2),
        DataRxDutyTime(3),
        DataRxNumValidPackets(4),
        DataRxNumFailedPackets(5),
        NumDataRxFields(6);

        private final int value;

        DataRxEnum(int value) {
            this.value = value;
        }
    }

    public enum DataTxEnum {
        DataTxControlThrust(0),
        DataTxControlYaw(1),
        DataTxControlPitch(2),
        DataTxControlRoll(3),
        NumDataTxFields(4);

        private final int value;

        DataTxEnum(int value) {
            this.value = value;
        }
    }

    public SerialEventManager(String port, int baudRate) {
        myPort = new Serial(port, baudRate);
        dataRx = new short[DataRxEnum.NumDataRxFields.value];
        dataTx = new short[DataTxEnum.NumDataTxFields.value];
        txBuffer = new byte[DataTxEnum.NumDataTxFields.value * 4 + 4];
        rxBuffer = new byte[RX_BUFFER_SIZE];
        packetBuffer = new byte[RX_BUFFER_SIZE];
    }

    public void initSerialEventManager() {
        System.out.println(DataRxEnum.NumDataRxFields.value);
    }

    public void readSerial() {

        bytesAvailable = myPort.available();

        if (0 < bytesAvailable)
        {
            bytesRead = myPort.readBytes(rxBuffer);
            //println(bytesRead);
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

    }

    private void parseData(byte[] rawPacket, int  length)
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
        byte receivedChecksum = rawPacket[packetPos-1];
        checksum -= receivedChecksum; //SUBTRACT THE received unescaped checksum
        if (receivedChecksum != checksum)
        {
            numFailedPackets++;
            return;
        }
        numValidPackets++;

        int packetLength = packetPos-1; //The length without the checksum

        //Transfer all data
        int dataField = 0;
        for (int i = 0; i < packetLength; i += 2)
        {
            dataRx[dataField] = (short)((rawPacket[i] << 8) | (rawPacket[i+1] & 0x000000FF)); //TODO:funkart?
            dataField++;
        }
    }

    public void writeSerial() {
        //Calc checksum on the fly
        byte checksum = 0;
        int packetPos = 0;

        txBuffer[packetPos++] = STX_BYTE; //Start byte

        for (int i = 0; i < DataTxEnum.NumDataTxFields.value; i++)
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

    private int escapeIfNeededAndIncrement(byte[] buffer, int pos)
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

    public short getRxFieldOfType(DataRxEnum type) {
        return dataRx[type.value];
    }

    public void setTxFieldOfType(DataTxEnum type, short value) {
        dataTx[type.value] = value;
    }

    public void stop() {
        myPort.dispose();
    }

}
