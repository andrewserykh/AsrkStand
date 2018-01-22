#ifndef ETH0_H_
#define ETH0_H_

#ifdef ARDUINO

  //---Modbus
  Mb.Run();
  converter conv;
  Mb.R[1] = conv.float2int1(devAsrk[0].value1); //40002
  Mb.R[2] = conv.float2int2(devAsrk[0].value1);
  Mb.R[3] = conv.float2int1(devAsrk[0].value2); //40004
  Mb.R[4] = conv.float2int2(devAsrk[0].value2);
  Mb.R[5] = conv.float2int1(devAsrk[0].value3); //40006
  Mb.R[6] = conv.float2int2(devAsrk[0].value3);
  Mb.R[7] = (int)devAsrk[0].isactive;           //40008
  
  Mb.R[11] = conv.float2int1(devAsrk[1].value1);
  Mb.R[12] = conv.float2int2(devAsrk[1].value1);
  Mb.R[13] = conv.float2int1(devAsrk[1].value2);
  Mb.R[14] = conv.float2int2(devAsrk[1].value2);
  Mb.R[15] = conv.float2int1(devAsrk[1].value3);
  Mb.R[16] = conv.float2int2(devAsrk[1].value3);
  Mb.R[17] = (int)devAsrk[1].isactive;
  
  Mb.R[21] = conv.float2int1(devAsrk[2].value1);
  Mb.R[22] = conv.float2int2(devAsrk[2].value1);
  Mb.R[23] = conv.float2int1(devAsrk[2].value2);
  Mb.R[24] = conv.float2int2(devAsrk[2].value2);
  Mb.R[25] = conv.float2int1(devAsrk[2].value3);
  Mb.R[26] = conv.float2int2(devAsrk[2].value3);
  Mb.R[27] = (int)devAsrk[2].isactive;

  Mb.R[31] = conv.float2int1(devAsrk[3].value1);
  Mb.R[32] = conv.float2int2(devAsrk[3].value1);
  Mb.R[33] = conv.float2int1(devAsrk[3].value2);
  Mb.R[34] = conv.float2int2(devAsrk[3].value2);
  Mb.R[35] = conv.float2int1(devAsrk[3].value3);
  Mb.R[36] = conv.float2int2(devAsrk[3].value3);
  Mb.R[37] = (int)devAsrk[3].isactive;

  Mb.R[41] = conv.float2int1(devAsrk[4].value1);
  Mb.R[42] = conv.float2int2(devAsrk[4].value1);
  Mb.R[43] = conv.float2int1(devAsrk[4].value2);
  Mb.R[44] = conv.float2int2(devAsrk[4].value2);
  Mb.R[45] = conv.float2int1(devAsrk[4].value3);
  Mb.R[46] = conv.float2int2(devAsrk[4].value3);
  Mb.R[47] = (int)devAsrk[4].isactive;

#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif
