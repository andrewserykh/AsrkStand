/*
* - Класс для конвертации типов
*/

#ifndef converter_h
#define converter_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif 

class converter
{
        public:
          int float2int1(float f);
          int float2int2(float f);
 	private:
};

#endif // #ifndef converter_h
