#include "Arduino.h"
#include "converter.h"

int converter::float2int1(float f)
{
  union {
    char bytes[4];
    float fval;
  } type_converter;
  type_converter.fval = f;  
  int i=0;
  i = (byte)type_converter.bytes[3];
  i = i <<8;
  i+=(byte)type_converter.bytes[2];
  return(i);
}

int converter::float2int2(float f)
{
  union {
    char bytes[4];
    float fval;
  } type_converter;
  type_converter.fval = f;  
  int i=0;
  i = (byte)type_converter.bytes[1];
  i = i <<8;
  i+=(byte)type_converter.bytes[0];
  return(i);
}
