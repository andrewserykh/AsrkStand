#include "Arduino.h"
#include "dibus.h"

//---получить из пакета значение измеряемой величины
float dibus::value() {
  float value = -1;
  union {
    char bytes[4];
    float fval;
  } type_float;

  union {
    char bytes[2];
    int  ival;
  } type_int;

  if ((packetin[6] == 0x07) && (packetin[7] == 0x01)) { //одна величина float
    //---для БДМГ-300
    type_float.bytes[0] = packetin[15];
    type_float.bytes[1] = packetin[16];
    type_float.bytes[2] = packetin[17];
    type_float.bytes[3] = packetin[18];
    return (type_float.fval);
  } // if ==0x07 & 0x01

  if ((packetin[6] == 0x07) && (packetin[7] == 0x7d)) { //одна величина float
    //---для ДБГ-С11Д
    byte input_value[2];
    input_value[0] = packetin[22];
    input_value[1] = packetin[23];
    type_int.bytes[0] = input_value[0];
    type_int.bytes[1] = (byte)(3 & input_value[1]); // 3=00000011b
    int mantissa = type_int.ival;
    type_int.bytes[0] = (input_value[1] & 252); // 252=11111100b
    type_int.bytes[0] = (uint8_t)type_int.bytes[0] >> 2;
    type_int.bytes[1] = (byte)0x00;
    int stage = type_int.ival;
    if (stage > 32) stage = (64 - stage) * -1;
    value = ((mantissa) / 100.0) * pow(10, stage);
    return (value);
  } // if ==0x07 & 0x01
  return value;
}


//---Функция запроса показаний из БДМГ300
//FF FF FF 01 01 01 06 0B 01 00 6F 44 74 80 23 23 00 00 00 01 01 01
bool dibus::getvalue(byte a1, byte a2, byte a3) {
  packetout[0] = a1;
  packetout[1] = a2;
  packetout[2] = a3;
  packetout[3] = 0x01;
  packetout[4] = 0x01;
  packetout[5] = 0x01;
  packetout[6] = 0x06; //06-получить данные
  packetout[7] = 0x7d; //0b-тип данных для БДМГ
  packetout[8] = 0x01;
  packetout[9] = 0x00; //конец заголовка
  long crc = _crc(packetout, 10);
  packetout[10] = (byte)(crc & 0xFF);
  packetout[11] = (byte)((crc >> 8) & 0xFF);
  packetout[12] = (byte)((crc >> 16) & 0xFF);
  packetout[13] = (byte)((crc >> 24) & 0xFF);
  packetout[14] = 0x00;
  packetout[15] = 0x00;
  packetout[16] = 0x00;
  packetout[17] = 0x00;
  packetout[18] = 0x00;
  packetout_len = 19;
  txdelay = 20;
  return true;
}

bool dibus::getlink(byte a1, byte a2, byte a3) {
  packetout[0] = a1; //Addr1
  packetout[1] = a2; //Addr2
  packetout[2] = a3; //Addr3
  packetout[3] = 0x01; //от Adr1
  packetout[4] = 0x01; //от Adr2
  packetout[5] = 0x01; //от Adr3
  packetout[6] = 0x04; // 0x06-получить данные
  packetout[7] = 0x01; //0b тип данных для ДБГ
  packetout[8] = 0x00;
  packetout[9] = 0x00;
  long crc = _crc(packetout, 10);
  packetout[10] = (byte)(crc & 0xFF);
  packetout[11] = (byte)((crc >> 8) & 0xFF);
  packetout[12] = (byte)((crc >> 16) & 0xFF);
  packetout[13] = (byte)((crc >> 24) & 0xFF);
  packetout_len = 14;
  txdelay = 20;
  return true;
}

bool dibus::setsign(byte a1, byte a2, byte a3, byte color) {
  packetout[0] = a1; //Addr1
  packetout[1] = a2; //Addr2
  packetout[2] = a3; //Addr3
  packetout[3] = 0x01; //от Adr1
  packetout[4] = 0x01; //от Adr2
  packetout[5] = 0x01; //от Adr3
  packetout[6] = 0x08;
  packetout[7] = 0x01;
  packetout[8] = 0x08;
  packetout[9] = 0x00;
  {
    long crc = _crc(packetout, 10);
    packetout[10] = (byte)(crc & 0xFF);
    packetout[11] = (byte)((crc >> 8) & 0xFF);
    packetout[12] = (byte)((crc >> 16) & 0xFF);
    packetout[13] = (byte)((crc >> 24) & 0xFF);
  }
  byte data[12];
  data[0] = 0x02;
  data[1] = 0x40;
  data[2] = 0x00;
  data[3] = 0x03;
  if (color == RED) {
    data[4] = 0x01;
  } else {
    data[4] = 0x00;
  }
  if (color == GREEN) {
    data[5] = 0x01;
  } else {
    data[5] = 0x00;
  }
  if (color == YELLOW) {
    data[6] = 0x01;
  } else {
    data[6] = 0x00;
  }
  if (color == RED) {

    data[7] = 0x01;
  } else {
    data[7] = 0x00;
  }
  {
    long crc = _crc(data, 8);
    data[8] = (byte)(crc & 0xFF);
    data[9] = (byte)((crc >> 8) & 0xFF);
    data[10] = (byte)((crc >> 16) & 0xFF);
    data[11] = (byte)((crc >> 24) & 0xFF);
  }
  for (int i = 0; i < 12; i++) packetout[14 + i] = data[i];
  txdelay = 30;
  packetout_len = 26;
  return true;
}


//---проверка crc пакета
bool dibus::ispacket() {
  bool ispkt = false;
  if (packetin_len < 6) return false; // длина dibus пакета как минимум 6 байт
  long crc = _crc(packetin, 10);

  if (
    (byte)(crc & 0xFF) == (byte)packetin[10] &&
    (byte)((crc >> 8) & 0xFF) == (byte)packetin[11] &&
    (byte)((crc >> 16) & 0xFF) == (byte)packetin[12] &&
    (byte)((crc >> 24) & 0xFF) == (byte)packetin[13]) ispkt = true;
  return ispkt;
}

//---для отладки:вывод на Serial консоль содержимого массива packet
void dibus::showpacket() {
  for (int i = 0; i < packetin_len; i++) {
    Serial.print((byte)packetin[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
}

/*
Формат пакета Dibus
Адрес   Адрес  Тип   Тип    Размер      CRC
Получ.  Отпр. пакета данных данных
0 1 2 | 3 4 5 | 6    | 7    | 8 9 | 10 11 12 13 | .. (Блок данных)
*/

//---получить тип прибора из пакета
int dibus::type() {
  int type = 0;
  if ((packetin[6] == 0x07) && (packetin[7] == 0x01)) return (BDMG300);
  if ((packetin[6] == 0x07) && (packetin[7] == 0x7d)) return (DBGS11D);
  if ((packetin[6] == 0x03) && (packetin[7] == 0x00)) return (BAS1S);
  return type;
}


byte dibus::a1() {
  return (packetin[3]);
}
byte dibus::a2() {
  return (packetin[4]);
}
byte dibus::a3() {
  return (packetin[5]);
}

long dibus::_crc (byte b[], int len) {
  long CRC = 0;
  int DataSize = len;
  int i = 0;
  if (DataSize % 2 == 1) {
    CRC = CRC ^ (byte)b[i];
    i++;
  } //if %2
  while (i < DataSize) {
    byte byte_i = (byte)b[i];
    byte byte_i1 = (byte)b[i + 1];
    CRC =  (CRC << 5) | (CRC >> (32 - 5)); // циклический сдвиг влево на 5 бит
    CRC = CRC ^ (((long)byte_i << 8) + (long)byte_i1 ); //сложение XOR со сдвигом
    i++;
    i++;
  } // while
  return (CRC);
}

