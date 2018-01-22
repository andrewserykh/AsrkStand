/*
* - Функции протокола MODBUS
*/

#ifndef modbusrtu_h
#define modbusrtu_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class modbusrtu
{
  public:
    int txdelay; //задержка после отправки пакета
    byte packet[96]; //принятый пакет (макс 96 байт)
    int packet_length; //длина принятого пакета
    byte packetout[64]; //пакет на отправку
    int packetout_len; //длина пакета на отправку
    byte search; //хранение адреса пр поиске
    bool make(byte adr, byte func, int start, int len); //сделать пакет модбас
    bool ispacket(); //проверяет по crc что пакет верный
    byte adr(); //получить адрес из пакета
    byte fn(); //получить номер функции из пакета
    int length(); //длина данных в пакете
    float getfloat(int start); //получить float величину из пакета
  private:
    unsigned int _crc(byte pkt[], int len);
};

#endif // #ifndef modbusrtu_h
