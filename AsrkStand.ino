/* v2.32
   _______   _________    RS-485:
  |       | | C    C  |   COM0 - Dx=[28]
  |       | | O    O  |   COM1 - Dx=[24]
  | MEGA  | | M    M  |   COM2 - Dx=[22]
  | 2650  | | 0    3  |   COM3 - Dx=[26]
  |       | |         |   COM4 - Dx=[29]
  |_______| | C    C  |
   _________| O    O  |   RS-232:
  |           M    M  |   COM5 - RX=[A15] ; TX=[25]
  | [ COM4 ]  1    2  |
  |___________________|

	EEPROM:
	----------------
	0	 ip1 addr
	1	 ip2 addr
	2	 ip3 addr
	3	 ip4 addr
	4	 mb addr slave
	5	 mode
  6  protocol ch1
  7  protocol ch2
  8  protocol ch3
  9  protocol ch4
  10 protocol ch5
  11 mbadr ch1
  12 mbadr ch2
  13 mbadr ch3
  14 mbadr ch4
  15 mbadr ch5
  16 signalisator status (GREEN,YELLOW,RED)

*/
//#include <avr/pgmspace.h> //если используется PROGMEM
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h>
#include "Mudbus.h"
#include "dibus.h"
#include "devasrk.h"
#include "hmi.h"
#include "converter.h"
#include "modbusrtu.h"
//#include "eth_const.h"

//---Типы приборов (также описаны в dibus.h)
#define UNDEFINE   0  //не определен
#define BDMG300    1  //БДМГ-3000
#define DBGS11D    2  //ДБГ-С11Д
#define UDA1AB     3  //УДА-1АБ
#define UDMN100PD  4  //УДМН-100ПД
#define UDPN       5  //УДПН
#define BAS1S      6  //БАС-1С
#define DUGA       7  //ИНТРА ДУГА
#define BRIG       8  //ИНТРА БРИГ
#define DCON       9  //СИГНАЛИЗАТОР DCON 
//---Режимы работы
#define BROADCAST  0  //широковещательный запрос
#define REQUEST    1  //опрос по адресу
//---Протоколы связи (также описаны в devasrk.h)
#define AUTO       0
#define DIBUS      1
#define MODBUS     2
#define INTRA      3   //интра (скорость порта 115200)
#define SIGNAL     4   //светофор (только TX)
#define SIGNDCON   5   //светофор стендов ИНТРА
#define SIMULATOR  6   //имитатор устройства
//---Распиновка последовательных портов RS-485
#define RS485Transmit    HIGH
#define RS485Receive     LOW
#define RsDir0  28    //RS485 module 0 (28)
#define RsDir1  24    //RS485 module 1 (24)
#define RsDir2  22    //RS485 module 2 (22)
#define RsDir3  26    //RS485 module 3 (26)
#define RsDir4  29    //RS485 module 4 (29)
#define STX4    A10   //Software TX module 4 (22)
#define SRX4    A11   //Software RX module 4 (A8)

#define STX5    A8    //Software TX module 5 (23)
#define SRX5    A9    //Software RX module 5 (A15)
//---
#define GREEN   1
#define YELLOW  2
#define RED     3

void(* resetArduino) (void) = 0;  //необходимо для перезагрузки

bool Debug = false; //выводить в Serial отладочную информацию

SoftwareSerial SSerial4(SRX4, STX4); //программные последовательные порты
SoftwareSerial SSerial5(SRX5, STX5);

char SerialIn[7][96]; //буфер приема по serial портам
byte SerialInLen[7]; //заполнение буфера
long SerialMillisRcv[7]; //прием по 485 порту (отсрочка на прием всего пакета)

Mudbus Mb;
int mbAddr;

hmi Hmi;
dibus devDibus;
modbusrtu devModbus;
devasrk devAsrk[5];
byte SignStatus;

//---Настройки Ethernet
EthernetServer server(80);
byte mac[] = { 0xc0, 0xff, 0xee, 0x01, 0x01, 0x02 }; //базовый mac адрес
String LanString = "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
long LanMillis;

void setup() {
  pinMode(13, OUTPUT); digitalWrite(13, HIGH); //встроенный led L
  Serial.begin(9600); //RS-232 COM0 Debug
  if (Debug) Serial.println("");
  if (Debug) Serial.println("Init..");
  pinMode(RsDir0, OUTPUT); // TX Control
  pinMode(RsDir1, OUTPUT); // TX Control
  pinMode(RsDir2, OUTPUT); // TX Control
  pinMode(RsDir3, OUTPUT); // TX Control
  pinMode(RsDir4, OUTPUT); // TX Control
  SSerial5.begin(9600); // RS-232 Nextion
  byte fin[] = {0xff, 0xff, 0xff}; //добивка для приема экраном команды
  SSerial5.print("ta0.txt=\"Loading..\"");
  SSerial5.write(fin, 3);

  if (Debug) Serial.print("Load cfg..");
  mbAddr = EEPROM.read(4); //считываем Modbus адрес из flash
  for (int i = 0; i < 5; i++) {
    devAsrk[i].autostart = true;
    devAsrk[i].interval = 1000 * 2 + (i * 200); //интервал опроса 2000,2100,2200,.. раскидать опрос
    devAsrk[i].protocol = EEPROM.read(6 + i); // 6,7,8,9,10
    if(devAsrk[i].protocol>10) devAsrk[i].protocol = AUTO;
    devAsrk[i].mbadr = EEPROM.read(11 + i); // 11,12,13,14
    if(devAsrk[i].mbadr==255) devAsrk[i].mbadr = 1;
  }
  SignStatus = EEPROM.read(16);
  if (SignStatus > RED) { //проверка корректности записанного состояния светофора
    SignStatus = GREEN;
    EEPROM.write(16, SignStatus);
  }
  if (Debug) Serial.println("ok");

  long baud = 9600;
  if (devAsrk[1].protocol == INTRA) baud = 115200;
  if (devAsrk[1].protocol == SIGNDCON) baud = 57600;
  
  Serial1.begin(baud);  // RS-485-1 module
  if (Debug) Serial.println(baud);
  baud = 9600;
  if (devAsrk[2].protocol == INTRA) baud = 115200;
  if (devAsrk[2].protocol == SIGNDCON) baud = 57600;  
  Serial2.begin(baud);  // RS-485-2 module
  if (Debug) Serial.println(baud);
  baud = 9600;
  if (devAsrk[3].protocol == INTRA) baud = 115200;
  if (devAsrk[3].protocol == SIGNDCON) baud = 57600;  
  Serial3.begin(baud);  // RS-485-3 module
  if (Debug) Serial.println(baud);
  baud = 9600;
  if (devAsrk[4].protocol == INTRA) baud = 115200;
  if (devAsrk[4].protocol == SIGNDCON) baud = 57600;  
  SSerial4.begin(baud); // RS-485-4 module
  if (Debug) Serial.println(baud);
  if (Debug) {
    Serial.print("EEPROM ip:"); Serial.print(EEPROM.read(0)); Serial.print(".");
    Serial.print(EEPROM.read(1)); Serial.print("."); Serial.print(EEPROM.read(2));
    Serial.print("."); Serial.println(EEPROM.read(3));
  }
  //считываем ip адрес из памяти
  int tmpip[4];
  tmpip[0] = EEPROM.read(0);
  tmpip[1] = EEPROM.read(1);
  tmpip[2] = EEPROM.read(2);
  tmpip[3] = EEPROM.read(3);
  //проверяем корректность считанного ip адреса
  if (tmpip[0]==255 ||tmpip[3]==0){
    if (Debug) Serial.println("Default IP");
    tmpip[0]=192;
    tmpip[1]=168;
    tmpip[2]=0;
    tmpip[3]=8; 
  }
  IPAddress ip(tmpip[0], tmpip[1], tmpip[2], tmpip[3]); //устанавливаем ip адрес
  //выставляем последние два байта mac адреса случайной величиной  //выставляем последние два байта mac адреса случайной величиной
  int mac3 = 1; int mac45 = 1; for (int i = 0; i < 5000; i++) {
    mac45 = mac45 + analogRead(A0); mac3 = mac3 - analogRead(A1);
  }
  mac[3] = mac3 & 0xFF; mac[4] = mac45 & 0xFF; mac[5] = (mac45 >> 8) & 0xFF;

  LanString = "";
  Ethernet.begin(mac, ip);
  server.begin();
  delay(1000);
  if (Debug) {
    Serial.print("Server "); Serial.print(Ethernet.localIP());
    Serial.println("..run");
  }
  digitalWrite(13, LOW);
}

void loop() {
  while (1) {
    //==============================================НАЧАЛО АЛГОРИТМА===============================================================
    //---Опрос приборов
    for (int i = 0; i < 5; i++) { //перебор всех приборов
      if (millis() - devAsrk[i].millis > devAsrk[i].interval) { //отправляем запрос данных
        //SerialSend(i + 1, devDibus.setsiren(0xe9, 0xbf, 0x93)); //отправляем широковещательный запрос ping
        //SerialSend(i + 1, devDibus.setsiren(0xff, 0xff, 0xff)); //отправляем широковещательный запрос ping

        if (devAsrk[i].protocol == AUTO) {
          //devDibus.getvalue(0xff, 0xff, 0xff);
          //SerialSend(i, devDibus.packetout, devDibus.packetout_len);
          if(Debug){Serial.print("Send COM"); Serial.print(i); Serial.print(", len="); Serial.println(devDibus.packetout_len);}
        }//protocol = AUTO

        if (devAsrk[i].protocol == DIBUS) {
          if (devAsrk[i].mode == BROADCAST) {
            devDibus.getvalue(0xff, 0xff, 0xff);
            SerialSend(i, devDibus.packetout, devDibus.packetout_len, devDibus.txdelay);
            devAsrk[i].sended();
          }//mode=BROADCAST
          if (devAsrk[i].mode == REQUEST) {
            devDibus.getvalue(devAsrk[i].dbadr[0], devAsrk[i].dbadr[1], devAsrk[i].dbadr[2]);
            SerialSend(i, devDibus.packetout, devDibus.packetout_len, devDibus.txdelay);
            devAsrk[i].sended();
          }//mode=REQUEST
        }//protocol=DIBUS

        if (devAsrk[i].protocol == MODBUS) {
          if (devAsrk[i].mode == BROADCAST) {
            devModbus.make(devModbus.search, 4, 0, 9);
            SerialSend(i, devModbus.packetout, devModbus.packetout_len, devModbus.txdelay);
            devAsrk[i].sended();
            devModbus.search++;
            if (devModbus.search > 0x7f) devModbus.search = 0x01;
          }//mode=BROADCAST поиск адреса
          if (devAsrk[i].mode == REQUEST) {
            devModbus.make(devAsrk[i].mbadr, 4, 0, 9);
            SerialSend(i, devModbus.packetout, devModbus.packetout_len, devModbus.txdelay);
            devAsrk[i].sended();
          }//mode=REQUEST
        }//protocol=MODBUS

        if (devAsrk[i].protocol == INTRA) {
          if (devAsrk[i].mode == BROADCAST) {
            devDibus.broadcastintra();
            SerialSend(i, devDibus.packetout, devDibus.packetout_len, devDibus.txdelay);
            devAsrk[i].sended();
          }//mode=BROADCAST
          if (devAsrk[i].mode == REQUEST) {
            devDibus.getintra(devAsrk[i].dbadr[0], devAsrk[i].dbadr[1], devAsrk[i].dbadr[2]);
            SerialSend(i, devDibus.packetout, devDibus.packetout_len, devDibus.txdelay);
            devAsrk[i].sended();
          }//mode=REQUEST
        }//protocol=INTRA

        if (devAsrk[i].protocol == SIGNAL) {
          devDibus.setsign(0xff, 0xff, 0xff, SignStatus);
          SerialSend(i, devDibus.packetout, devDibus.packetout_len, devDibus.txdelay);
          devAsrk[i].sended();
        }//protocol=SIGNAL

        if (devAsrk[i].protocol == SIGNDCON) {
          String strDcon;
          if (SignStatus==GREEN) strDcon = "#01A001#01A100#01A200";
          if (SignStatus==YELLOW) strDcon = "#01A000#01A101#01A200";
          if (SignStatus==RED) strDcon = "#01A000#01A100#01A201";
          SerialSendStr(i, strDcon, 10);
          devAsrk[i].sended();
          if (Debug) Serial.print("Sign:"); 
          if (Debug) Serial.println(SignStatus);
        }//protocol=SIGNDCON

        if (devAsrk[i].protocol == SIMULATOR) {
          devAsrk[i].value1 = 0.00000001F + ( 0.0000000001F * random(100));
        }//protocol=SIMULATOR

        devAsrk[i].millis = millis();
      }//if millis
    } //for (i=0..5)

#include "modbus0.h"; //работа с modbus tcp

#include "eth0.h"; //работа с ethernet

#include "serial0.h"; //прием данных по последовательному порту

    if (millis() - Hmi.millis > 5000) { //отправляем данные на экран
      byte fin[] = {0xff, 0xff, 0xff}; //добивка для приема экраном команды
      SSerial5.print("ta0.txt=\"");
      SSerial5.print(Ethernet.localIP());
      SSerial5.print("\"");
      SSerial5.write(fin, 3);
      for (int h = 0; h < 5; h++) {
        SSerial5.print("tc");
        SSerial5.print(h);
        SSerial5.print(".txt=\"");
        if (Hmi.step == 0) {
          SSerial5.print(devAsrk[h].valuehuman(1));
        }
        if (Hmi.step == 1) {
          SSerial5.print(devAsrk[h].typehuman_en());
        }
        if (Hmi.step == 2) {
          SSerial5.print(devAsrk[h].adrhuman());
        }
        SSerial5.print("\"");
        SSerial5.write(fin, 3);
      }//for h
      Hmi.step++;
      if (Hmi.step > 2) Hmi.step = 0;
      Hmi.millis = millis();
    }//hmi.millis


    //==============================================КОНЕЦ АЛГОРИТМА================================================================
  }
}

//---Универсальные функции отправки пакета по RS-486 порту
void SerialSend(int Port, byte packet[], byte len, int txdelay) {
  digitalWrite(13, HIGH); //индикация отправки встроенным L led
  switch (Port) {
    case 1:
      digitalWrite(RsDir1, RS485Transmit);
      Serial1.write(packet, len);
      delay(txdelay);
      digitalWrite(RsDir1, RS485Receive);
      break;
    case 2:
      digitalWrite(RsDir2, RS485Transmit);
      Serial2.write(packet, len);
      delay(txdelay);
      digitalWrite(RsDir2, RS485Receive);
      break;
    case 3:
      digitalWrite(RsDir3, RS485Transmit);
      Serial3.write(packet, len);
      delay(txdelay);
      digitalWrite(RsDir3, RS485Receive);
      break;
    case 4:
      digitalWrite(RsDir4, RS485Transmit);
      SSerial4.write(packet, len);
      delay(txdelay);
      digitalWrite(RsDir4, RS485Receive);
      break;
  }
  digitalWrite(13, LOW);
}

void SerialSendStr(int Port, String packet, int txdelay) {
  digitalWrite(13, HIGH); //индикация отправки встроенным L led
  switch (Port) {
    case 1:
      digitalWrite(RsDir1, RS485Transmit);
      Serial1.print(packet);
      delay(txdelay);
      digitalWrite(RsDir1, RS485Receive);
      break;
    case 2:
      digitalWrite(RsDir2, RS485Transmit);
      Serial2.print(packet);
      delay(txdelay);
      digitalWrite(RsDir2, RS485Receive);
      break;
    case 3:
      digitalWrite(RsDir3, RS485Transmit);
      Serial3.print(packet);
      delay(txdelay);
      digitalWrite(RsDir3, RS485Receive);
      break;
    case 4:
      digitalWrite(RsDir4, RS485Transmit);
      SSerial4.print(packet);
      delay(txdelay);
      digitalWrite(RsDir4, RS485Receive);
      break;
  }
  digitalWrite(13, LOW);
}
