#ifndef __SERIAL0_H__
#define __SERIAL0_H__

#ifdef ARDUINO

//---Прием данных по последовательным портам

while (Serial.available()) {
  char SerialChar = (char)Serial.read();
  if (SerialInLen[0]==0 && SerialChar == 0x00) return; //обработка помех
  SerialIn[0][SerialInLen[0]] = SerialChar;
  SerialInLen[0]++;
  SerialMillisRcv[0] = millis(); //для отсрочки обработки пакета 485 порта
}

while (Serial1.available()) {
  char SerialChar = (char)Serial1.read();
  if (SerialInLen[1]==0 && SerialChar == 0x00) return; //обработка помех
  SerialIn[1][SerialInLen[1]] = SerialChar;
  SerialInLen[1]++;
  SerialMillisRcv[1] = millis(); //для отсрочки обработки пакета 485 порта
}

while (Serial2.available()) {
  char SerialChar = (char)Serial2.read();
  if (SerialInLen[2]==0 && SerialChar == 0x00) return; //обработка помех
  SerialIn[2][SerialInLen[2]] = SerialChar;
  SerialInLen[2]++;
  SerialMillisRcv[2] = millis();
}

while (Serial3.available()) {
  char SerialChar = (char)Serial3.read();
  if (SerialInLen[3]==0 && SerialChar == 0x00) return; //обработка помех
  SerialIn[3][SerialInLen[3]] = SerialChar;
  SerialInLen[3]++;
  SerialMillisRcv[3] = millis();
}

SSerial4.listen();
do { //оттяжечка на прием по софт порту
  while (SSerial4.available()) {
    char SerialChar = (char)SSerial4.read();
    if (SerialInLen[4]==0 && SerialChar == 0x00) return; //обработка помех
    SerialIn[4][SerialInLen[4]] = SerialChar;
    SerialInLen[4]++;
    SerialMillisRcv[4] = millis();
  }
} while (millis() - SerialMillisRcv[4] < 100);

//-----------------------------------------------------------------------------------------------------Есть данные по 485 порту

for (int n = 0; n < 5; n++) { //перебор массива принятых пакетов (0,1,2,3,4)

  if (SerialInLen[n] > 0 && (millis() - SerialMillisRcv[n] > 100)) { //оттяжка на прием пакета целиком (для dibus = 100мс)
    devAsrk[n].isactive = true;

    //04 04 04 01 01 01
    if (SerialInLen[n] > 6 && SerialInLen[n] < 96) { //предположительно по размеру это пакет

  if(Debug){
    Serial.print("COM");
    Serial.println(n);
    Serial.println("#1 is packet");
    for (int q = 0; q < SerialInLen[n]; q++) { //вывод принятого пакета на консоль
    Serial.print((byte)SerialIn[n][q], HEX); Serial.print(" ");
    } Serial.println(" ");
  }
      if (SerialIn[n][0] == 0x01 && SerialIn[n][1] == 0x01 && SerialIn[n][2] == 0x01) { //предположительно это dibus начало 01 01 01
  if(Debug) Serial.println("#2 maybe dibus");
        //---прверка на тип пакета dibus
        for (int i = 0; i < SerialInLen[n]; i++) devDibus.packetin[i] = (byte)SerialIn[n][i]; //заполняем массив класса
        devDibus.packetin_len = SerialInLen[n];
        if (devDibus.ispacket()) { //принятые данные являются пакетом dibus
  if(Debug) Serial.println("#3 is dibus");
          devAsrk[n].protocol = DIBUS;
          devAsrk[n].mode = REQUEST;
          devAsrk[n].dbadr[0] = devDibus.a1();
          devAsrk[n].dbadr[1] = devDibus.a2();
          devAsrk[n].dbadr[2] = devDibus.a3();
          devAsrk[n].type = devDibus.type();
          if (devAsrk[n].type==BDMG300){
              devAsrk[n].value1 = devDibus.value();
              devAsrk[n].isactive=true;
              if(Debug)Serial.println(devAsrk[n].valuehuman(1));          
          }//type=BDMG300
          if (devAsrk[n].type==DBGS11D){
              devAsrk[n].value1 = devDibus.value();
              devAsrk[n].isactive=true;
              if(Debug)Serial.println(devAsrk[n].valuehuman(1));          
          }//type=DBGS11D
          if (devAsrk[n].type==BAS1S){
              devAsrk[n].protocol = SIGNAL;
              devAsrk[n].isactive=true;
              if(Debug)Serial.println("BAS1S");
          }//type=BAS1S
          devAsrk[n].recieved();
        } else { //ispkt
          //!ИСКЛЮЧЕНИЕ: специально для БАС-1С у него странно расчитана crc заголовка
          if (devDibus.packetin[0] == 0x01 && devDibus.packetin[1] == 0x01 && devDibus.packetin[2] == 0x01 && devDibus.packetin[6] == 0xf9 && devDibus.packetin[7] == 0xff) {
//            Serial.println("#4 exception is dibus");
            devAsrk[n].protocol = DIBUS;
            devAsrk[n].mode = REQUEST;
            devAsrk[n].dbadr[0] = devDibus.a1();
            devAsrk[n].dbadr[1] = devDibus.a2();
            devAsrk[n].dbadr[2] = devDibus.a3();
            devAsrk[n].type = BAS1S;
            devAsrk[n].recieved();
            if(Debug)Serial.println("BAS1S");
          }//if
        }//ispkt
      }//if предположительно это dibus

      if (SerialIn[n][1]==0x04 || SerialIn[n][1]==0x03){ // f04 или f03, а может быть это modbus
        for (int i = 0; i < SerialInLen[n]; i++) devModbus.packet[i] = (byte)SerialIn[n][i]; //заполняем массив класса
        devModbus.packet_length = SerialInLen[n];
        if (devModbus.ispacket()) {//принятые данные являются пакетом dibus
          devAsrk[n].protocol = MODBUS;
          devAsrk[n].mode = REQUEST;
          devAsrk[n].mbadr = devModbus.adr();
          if(devModbus.length()==18) { //длина блока данных 18 байт
            devAsrk[n].type = UDA1AB;
            devAsrk[n].value1 = devModbus.getfloat(9);  //альфа
            devAsrk[n].value2 = devModbus.getfloat(3);  //бета
            devAsrk[n].value3 = devModbus.getfloat(15); //радон

            if(Debug)Serial.println(devAsrk[n].value1);
            if(Debug)Serial.println(devAsrk[n].value2);
            if(Debug)Serial.println(devAsrk[n].value3);
          }
          devAsrk[n].recieved();
        }//
      } // 0x04&0x12

    } //if length > <

    SerialInLen[n] = 0; //очистка буфера приема
  }
} //for (i0..4)
//---------------------------------------------------------------------------------------------------Есть данные по COM порту
/*
  if (SerialString[0].length() > 0) {
    Serial.println("Sending 485 request..");
    pinMode(RsDir1, OUTPUT);
    digitalWrite(RsDir1, RS485Transmit);
//    Serial1.print(devBdmg.getvalue(Bdmg1.addr[0], Bdmg1.addr[1], Bdmg1.addr[2]));
    delay(devBdmg.txdelay); //20
    digitalWrite(RsDir1, RS485Receive);

    //Serial.print(SerialString);
    SerialString = "";
  }
  */



#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif
