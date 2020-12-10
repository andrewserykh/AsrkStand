#ifndef __ETH0_H__
#define __ETH0_H__

#ifdef ARDUINO

if (millis() - LanMillis > 1000) {
  
  LanMillis = millis();

  EthernetClient client = server.available();
  client = server.available();

  if (client) {
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        if (LanString.length() > 499) LanString = "";
        LanString += c;
        if (c == '\n' && currentLineIsBlank) {
          //---Разбор принятых данных по Web
          if (LanString.indexOf("reset") > 0) {
            resetArduino();
          }
          if (LanString.indexOf("&fin=1") > 0) {
            String tempip = LanString.substring((LanString.indexOf("/?ip=") + 5), LanString.indexOf("&mb="));
            //---парсинг ip адреса
            int ip1 = tempip.substring(0, tempip.indexOf(".")).toInt();
            tempip = tempip.substring(tempip.indexOf(".") + 1, tempip.length());
            int ip2 = tempip.substring(0, tempip.indexOf(".")).toInt();
            tempip = tempip.substring(tempip.indexOf(".") + 1, tempip.length());
            int ip3 = tempip.substring(0, tempip.indexOf(".")).toInt();
            tempip = tempip.substring(tempip.indexOf(".") + 1, tempip.length());
            int ip4 = tempip.toInt();
            EEPROM.write(0, ip1); EEPROM.write(1, ip2); EEPROM.write(2, ip3); EEPROM.write(3, ip4);
            int temp = LanString.substring((LanString.indexOf("&mb=") + 4), LanString.indexOf("&mod1=")).toInt();
            EEPROM.write(4, temp);
            mbAddr = temp;
            temp = LanString.substring((LanString.indexOf("&pc0=") + 5), LanString.indexOf("&pc1=")).toInt();
            EEPROM.write(6, temp);
            devAsrk[0].protocol = temp;
            temp = LanString.substring((LanString.indexOf("&pc1=") + 5), LanString.indexOf("&pc2=")).toInt();
            EEPROM.write(7, temp);
            devAsrk[1].protocol = temp;
            temp = LanString.substring((LanString.indexOf("&pc2=") + 5), LanString.indexOf("&pc3=")).toInt();
            EEPROM.write(8, temp);
            devAsrk[2].protocol = temp;
            temp = LanString.substring((LanString.indexOf("&pc3=") + 5), LanString.indexOf("&pc4=")).toInt();
            EEPROM.write(9, temp);
            devAsrk[3].protocol = temp;
            temp = LanString.substring((LanString.indexOf("&pc4=") + 5), LanString.indexOf("&fin=")).toInt();
            EEPROM.write(10, temp);
            devAsrk[4].protocol = temp;
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          client.print("<!DOCTYPE HTML><html><head><meta charset=\"UTF-8\"><title>Стенд радиационного контроля</title></head><body><h2><a href='http://");
          client.print(Ethernet.localIP());
          client.print("'>ОБНОВИТЬ</a></h2>");
          client.print("<form name=\"f\" method=\"get\"><table style=\"border: 1px solid #aaa; background: #eee;\"><tr style=\"background: #ddd;\"><td colspan=\"2\">Состояние</td></tr>");
          for (int i = 0; i < 5; i++) {
            client.print("<tr><td>Канал ");
            client.print(i + 1);
            if (devAsrk[i].isactive) {
              client.print(" : ");
            } else {
              client.print(" <span style=\"font-size:10px;color:#cc0000;\">(не подключен)</span> ");
            }
            client.print("</td><td><b>");
            client.print(devAsrk[i].valuehuman(1));
            //client.print(devAsrk[i].valuenum1(i + 1)); client.print(","); client.print(devAsrk[i].valuenum2(i + 1)); client.print("e-"); client.print(devAsrk[i].valueexp(i + 1));
            client.print("</b> <span style=\"font-size:8px;\">");
            client.print(devAsrk[i].typehuman());
            client.print("</span>");

            client.print(" (");
            client.print(devAsrk[i].adrhuman());
            client.print(")");
            if (devAsrk[i].mode == BROADCAST && devAsrk[i].protocol != SIMULATOR) client.print(" ПОИСК");
            if (devAsrk[i].protocol == SIMULATOR) client.print(" ИМИТАТОР");

            client.print("</td></tr>");
          }
          client.print("<tr style=\"background: #ddd;\"><td colspan=\"2\">Настройки</td></tr>");
          client.print("<tr><td>MAC адрес</td><td>");
          client.print((byte)mac[0], HEX);
          client.print((byte)mac[1], HEX);
          client.print((byte)mac[2], HEX);
          client.print((byte)mac[3], HEX);
          client.print((byte)mac[4], HEX);
          client.print((byte)mac[5], HEX);
          client.print("</td></tr>");
          client.print("<tr><td>Сетевой адрес</td><td><input type=\"text\" name=\"ip\" value=\"");
          client.print(Ethernet.localIP());
          client.print("\">");
          client.print("</td></tr>");
          client.print("<tr><td>Modbus-slave адрес:</td><td><input type=\"text\" name=\"mb\" value=\"");
          client.print(mbAddr);
          client.print("\">");
          client.print("</td></tr>");
          client.print("<tr style=\"background: #ddd;\"><td colspan=\"2\">Интерфейсы</td></tr>");

          String chsel[10];
          for(int n=0;n<10;n++) chsel[n] = "";chsel[devAsrk[0].protocol] = "selected";
          client.print("<tr><td>Режим канал 1</td><td><select name=\"pc0\"><option value=\""+String(AUTO)+"\">авто</option><option value=\""+String(DIBUS)+"\"" + chsel[1] + ">dibus</option><option value=\""+String(MODBUS)+"\"" + chsel[2] + ">modbus</option><option value=\""+String(SIGNAL)+"\"" + chsel[3] + ">бас-1с</option><option value=\""+String(SIMULATOR)+"\"" + chsel[4] + ">имитатор</option></select></td></tr>");
          for(int n=0;n<10;n++) chsel[n] = "";chsel[devAsrk[1].protocol] = "selected";
          client.print("<tr><td>Режим канал 2</td><td><select name=\"pc1\"><option value=\""+String(AUTO)+"\">авто</option><option value=\""+String(DIBUS)+"\"" + chsel[1] + ">dibus</option><option value=\""+String(MODBUS)+"\"" + chsel[2] + ">modbus</option><option value=\""+String(INTRA)+"\"" + chsel[3] + ">интра</option><option value=\""+String(SIGNAL)+"\"" + chsel[4] + ">бас-1с</option><option value=\""+String(SIGNDCON)+"\"" + chsel[5] + ">светофор dcon</option><option value=\""+String(SIMULATOR)+"\"" + chsel[5] + ">имитатор</option></select></td></tr>");
          for(int n=0;n<10;n++) chsel[n] = "";chsel[devAsrk[2].protocol] = "selected";
          client.print("<tr><td>Режим канал 3</td><td><select name=\"pc2\"><option value=\""+String(AUTO)+"\">авто</option><option value=\""+String(DIBUS)+"\"" + chsel[1] + ">dibus</option><option value=\""+String(MODBUS)+"\"" + chsel[2] + ">modbus</option><option value=\""+String(INTRA)+"\"" + chsel[3] + ">интра</option><option value=\""+String(SIGNAL)+"\"" + chsel[4] + ">бас-1с</option><option value=\""+String(SIGNDCON)+"\"" + chsel[5] + ">светофор dcon</option><option value=\""+String(SIMULATOR)+"\"" + chsel[5] + ">имитатор</option></select></td></tr>");
          for(int n=0;n<10;n++) chsel[n] = "";chsel[devAsrk[3].protocol] = "selected";
          client.print("<tr><td>Режим канал 4</td><td><select name=\"pc3\"><option value=\""+String(AUTO)+"\">авто</option><option value=\""+String(DIBUS)+"\"" + chsel[1] + ">dibus</option><option value=\""+String(MODBUS)+"\"" + chsel[2] + ">modbus</option><option value=\""+String(INTRA)+"\"" + chsel[3] + ">интра</option><option value=\""+String(SIGNAL)+"\"" + chsel[4] + ">бас-1с</option><option value=\""+String(SIGNDCON)+"\"" + chsel[5] + ">светофор dcon</option><option value=\""+String(SIMULATOR)+"\"" + chsel[5] + ">имитатор</option></select></td></tr>");
          for(int n=0;n<10;n++) chsel[n] = ""; chsel[devAsrk[4].protocol] = "selected";
          client.print("<tr><td>Режим канал 5</td><td><select name=\"pc4\"><option value=\""+String(AUTO)+"\">авто</option><option value=\""+String(DIBUS)+"\"" + chsel[1] + ">dibus</option><option value=\""+String(MODBUS)+"\"" + chsel[2] + ">modbus</option><option value=\""+String(SIGNAL)+"\"" + chsel[4] + ">бас-1с</option><option value=\""+String(SIGNDCON)+"\"" + chsel[5] + ">светофор dcon</option><option value=\""+String(SIMULATOR)+"\"" + chsel[6] + ">имитатор</option></select></td></tr>");

          
          client.print("<tr style=\"background: #ddd;\"><td colspan=\"2\">Уставки</td></tr>");
          client.print("<tr><td>Канал 1.1</td><td><input type='text' name='c1' value='0'></td></tr>");
          
          
          client.print("<tr><td></td><td><a href='/reset'>перезапустить контроллер</a></td></tr>");
          client.print("<input type=\"hidden\" name=\"fin\" value=\"1\">");
          client.print("<tr><td></td><td><button type=\"submit\">Сохранить</button></td></tr>");
          client.print("</table></form>");
          client.print("</body></html>");

          LanString = "";
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
} //millis

#else
#error This is only for use on Arduino.
#endif // ARDUINO

#endif
