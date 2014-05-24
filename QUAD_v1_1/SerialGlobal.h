#ifndef SerialGlobal_h
#define SerialGlobal_h


#define RX_BUFFER_SIZE 64
#define STX_BYTE 0xAA
#define ESC_BYTE 0xBB
#define ETX_BYTE 0xCC

int numValidPackets = 0;
int numFailedPackets = 0;

enum DataRxEnum
{
  DataRxControlThrust,
  DataRxControlYaw,
  DataRxControlPitch,
  DataRxControlRoll,
  
  NumDataRxFields
};

int dataRx[NumDataRxFields];

enum DataTxEnum
{
  DataTxAngleX,
  DataTxAngleY,
  DataTxBatteryLevel,
  DataTxDutyTime,
  DataTxNumValidPackets,
  DataTxNumFailedPackets,
  
  NumDataTxFields
};

int dataTx[NumDataTxFields];

#endif
