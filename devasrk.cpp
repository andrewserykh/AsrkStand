#include "Arduino.h"
#include "devasrk.h"

//---получение значения из пакета в виде 1.2E-07
String devasrk::valuehuman(int channel) { //channel - выбор канала value1, value2, value 3
  float value = -1;
  if (channel==1) value=value1;
  if (channel==2) value=value2;
  if (channel==3) value=value3;
  if (channel==4) value=value4;
  
  String val;
  float f = 0;
  int e = 0;
  while (e < 32 && f < 1) {
    e++;
    f = value * pow(10, e);
  } //while
  if (f < 1) return ("err");
  val = String (f);
  val += "e-";
  val += String(e);
  return (val);
}

int devasrk::valuenum1(int channel) {
  float value = -1;
  switch (channel) {
    case 1:
      value = value1;
      break;
    case 2:
      value = value2;
      break;
    case 3:
      value = value3;
      break;
    case 4:
      value = value4;
      break;
  }//switch
  float f = 0;
  int e = 0;
  while (e < 32 && f < 1) {
    e++;
    f = value * pow(10, e);
  }
  return ((int)f);
}
int devasrk::valuenum2(int channel) {
  float value = -1;
  switch (channel) {
    case 1:
      value = value1;
      break;
    case 2:
      value = value2;
      break;
    case 3:
      value = value3;
      break;
    case 4:
      value = value4;
      break;
  }//switch
  float f = 0;
  int e = 0;
  while (e < 32 && f < 1) {
    e++;
    f = value * pow(10, e);
  }
  int i = (int)f;
  return ((int)(f * 100) - (i * 100));
}
int devasrk::valueexp(int channel) {
  float value = -1;
  switch (channel) {
    case 1:
      value = value1;
      break;
    case 2:
      value = value2;
      break;
    case 3:
      value = value3;
      break;
    case 4:
      value = value4;
      break;
  }//switch
  float f = 0;
  int e = 0;
  while (e < 32 && f < 1) {
    e++;
    f = value * pow(10, e);
  }
  if (e == 32) e = 0;
  return (e);
}

String devasrk::typehuman() {
  if (type == UNDEFINE) return ("НЕ ОПРЕДЕЛЕН");
  if (type == BDMG300) return ("БДМГ-300");
  if (type == DBGS11D) return ("ДБГ-С11Д");
  if (type == UDA1AB) return ("УДА-1АБ");
  if (type == UDMN100PD) return ("УДПН-100ПД");
  if (type == UDPN) return ("УДПН");
  if (type == BAS1S) return ("БАС-1С");
  return ("НЕ ОПРЕДЕЛЕН");
}

String devasrk::typehuman_en() {
  if (type == UNDEFINE) return ("-");
  if (type == BDMG300) return ("BDMG-300");
  if (type == DBGS11D) return ("DBG-S11D");
  if (type == UDA1AB) return ("UDA-1AB");
  if (type == UDMN100PD) return ("UDPN-100PD");
  if (type == UDPN) return ("UDPN");
  if (type == BAS1S) return ("BAS-1S");
  return ("-");
}

String devasrk::adrhuman() {
  String ret="";
  if (protocol == DIBUS) {
    ret+=(int)dbadr[0];
    ret+=(".");
    ret+=(int)dbadr[1];
    ret+=(".");
    ret+=(int)dbadr[2];
  }
  if (protocol == MODBUS) {
    if ((int)mbadr!=255) { ret+=(int)mbadr; } else { ret+="SEARCHING"; }
  }
  return ret;
}

void devasrk::sended() {
  if (pktsend && pktrecv == false) errlink++;
  if (errlink > errborder) isactive = false;
  pktsend = true;
  pktrecv = false;

}

void devasrk::recieved() {
  if (pktsend) {
    pktrecv = true; //если пакет был отправлен и принят, то взводим бит
    isactive = true;
    errlink = 0;
  }
}
