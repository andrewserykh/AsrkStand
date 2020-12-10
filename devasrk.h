/*
* - Класс для работы с приборами АСРК
*/

#ifndef bdmg_h
#define devasrk_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//---Типы приборов
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
//---Протоколы связи
#define AUTO       0
#define DIBUS      1
#define MODBUS     2
#define INTRA      3   //интра (скорость порта 115200)
#define SIGNAL     4   //светофор (только TX)
#define SIGNDCON   5   //светофор стендов ИНТРА
#define SIMULATOR  6   //имитатор устройства



class devasrk
{
  public:
    int 	   type;	     //тип устройства (БДМГ,ДБГ,УДА,УДМН,УДПН,..)
    int	     mode;	     //режим работы (запрос, broadcast,..)
    int      findtype;   //поиск типа прибора
    int      protocol;   //протокол связи (dibus, modbus,..)
    byte	   dbadr[3];   //dibus адрес устройства
    byte	   mbadr;	     //modbus адрес устройства
    bool	   isactive;   //устройство на связи
    bool	   autostart;  //производить опрос устройства автоматически
    float    value1;	   //измеряемая величина канал 1
    float    value2;	   //измеряемая величина канал 2
    float    value3;	   //измеряемая величина канал 3
    float    value4; 	   //измеряемая величина канал 4
    long     millis;     //хранение значения millis
    long     interval;   //интервал опроса ms
    int      errlink;    //счет ошибок связи
    int      errborder = 10; //максимальный счет ошибок
    void     sended();   //проверка состояния связи и обозначение отправки
    void     recieved(); //был принят пакет

    String   valuehuman(int channel);  //величина МЭД для вывода на экран 1.2e-7
    int      valuenum1(int channel); //вывод целой части показания (1.27e-07 выведет 1)
    int      valuenum2(int channel); //вывод десятичной части показаний (1.27e-07 выведет 7)
    int      valueexp(int channel); //вывод значения степени (1.27e-07 выведет 27)
    String   typehuman(); //вывод типа прибора русскими буквами
    String   typehuman_en(); //вывод типа прибора латиницей
    String   adrhuman(); //вывод адреса прибора строкой
  private:
    bool  pktsend;  //пакет отправлен
    bool  pktrecv;  //пакет принят
};

#endif // #ifndef devasrk_h
