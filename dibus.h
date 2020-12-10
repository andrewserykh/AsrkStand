/*
* - Функции протокола DIBUS (Tetra.ua)

_Структура пакета DIBUS:_

получа-  отправи- A  B  C  C     CRC      блок данных      CRC
тель     тель     |  |  |  |    /   \     0..327676 байт  /   \
FF FF FF 01 01 01 01 01 01 02 CR CR CR CR  00 00 00 ... CR CR CR CR
'----------заголовок 14 байт---------'  '-----блок данных------'
A - тип пакета
  0 = запрос на регистрацию ведомого
  1 = подтверждение ведомым приема команды
  2 = подтверждение регистрации ведомого
  3 = ошибка устройства
  4 = ping
  5 = ответ ведомого "хочу передать пакет"
  6 = получить данные у ведомого
  7 = запрошенные данные
  8 = передать данные ведомому
  ...
B - тип данных или интерфейс
  1 = байт, массив байтов
  3 = строка символов
  ...
C - размер данных (2 байта)

*/

#ifndef dibus_h
#define dibus_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

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

#define GREEN   1
#define YELLOW  2
#define RED     3

class dibus
{
  public:
    int txdelay; //задержка после отправки пакета
    byte packetin[96]; //принятый пакет (макс 96 байт)
    byte packetin_len; //длина принятого пакета
    byte packetout[64]; //пакет для отправки (макс 64 байта)
    byte packetout_len; //длина пакета для отправки
    boolean getvalue(byte a1, byte a2, byte a3); // запрос показаний гамма, адрес a1,a2,a3
    boolean getlink(byte a1, byte a2, byte a3); // запрос связи, адрес a1,a2,a3
    boolean setsign(byte a1, byte a2, byte a3, byte color); // установка светофора, адрес a1,a2,a3
    bool broadcastintra(); //широковещательный запрос устройств ИНТРА (ДУГА/БРИГ)
    bool getintra(byte a1, byte a2, byte a3); //получить показания из устройств ИНТРА (БРИГ/ДУГА)
    bool ispacket(); // является ли пакет в буфере пакетом dibus (проврка crc заголовка dibus)
    void showpacket(); //вывод на Serial консоль пакета в HEX представлении, используется для отладки
    float value(int index = 0); //получить из пакета значение измеряемой величины
    int type(); //получить тип прибора
    byte a1(); //первый байт dibus адреса
    byte a2(); //второй байт
    byte a3(); //третий байт адреса
  private:
    long _crc(byte b[], int len); //расчет crc
};

#endif // #ifndef dibus_h
