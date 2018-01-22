/*
* - Класс для работы с Nextion панелью
*/

#ifndef hmi_h
#define hmi_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif 

class hmi
{
        public:
          long millis;
          int step; //шаг вывода на экран (Показания->Адрес->Тип..)
          void put(String Object,String Text);
 	private:
};

#endif // #ifndef hmi_h
