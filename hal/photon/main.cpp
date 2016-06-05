#pragma SPARK_NO_PREPROCESSOR
#include <Particle.h>
#include <softap_http.h>
#include "http.h"

const char * DEVICE_TYPE = "relay";
const char * DEVICE_ID = "996903cc-f22a-4c03-94b1-fd050d942ef2";
const byte CURRENT_VERSION = 0;
const int EEPROM_VERSION = 0;
const int EEPROM_HOST_ADDR = 1;
const int EEPROM_SCHEDULE = 5;

struct Schedule
{
  char data[64];
};

STARTUP(softap_set_application_page_handler(http_handler, nullptr));

bool _connected = false;
TCPClient client;
void setup()
{
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

  byte version;
  EEPROM.get(EEPROM_VERSION, version);
  if (version != CURRENT_VERSION)
  {
    //TODO: Migrate somehow?
    WiFi.listen();
    EEPROM.put(EEPROM_VERSION, CURRENT_VERSION);
  }
  else
  {
      IpAddr ip;
      EEPROM.get(EEPROM_HOST_ADDR, ip);
      if (ip.addr[0] == 0xFF)
      {
        //If no address is set, start listening.
        WiFi.listen();
      }
      else if (!client.connect(ip.addr, 8081))
      {
        //If connecting failed, fall back to listening mode
        WiFi.listen();
      }
      else
      {
        //Tell the cloud what type of device this is
        client.printf("{\"type\":\"%s\", \"deviceId\":\"%s\"}",DEVICE_TYPE, DEVICE_ID);
        _connected = true;
      }
  }
}

char * processData(char * data, int count)
{
  char cmd = data[0];
  char * temp;
  char pin = 0;
  char * result = 0;
  Schedule schedule;
  switch (cmd)
      {
        case 'S':
        //write schedule
        memcpy(&schedule, &data[1], count - 1);
        EEPROM.put(EEPROM_SCHEDULE, schedule);
        break;

        case 'R':
        strtok(data, " ");
        temp = strtok(nullptr, " ");
        pin = atoi(temp);

        result = new char[2]();
        result[0] = digitalRead(0) == HIGH ? '1' : '0';
        break;

        case 'W':
        strtok(data, " ");
        temp = strtok(nullptr, " ");
        pin = atoi(temp);
        temp = strtok(nullptr, " ");

        digitalWrite(pin, atoi(temp) ? HIGH : LOW);
        break;

      }
    return result;
}

void loop()
{
  if (client.connected())
  {
    int total = client.available();
    if (total)
    {
      char * data = new char[total+1]();

      for (int i = 0; client.available(); ++i)
      {
        data[i] = client.read();
      }
      //Process Data
      char * result = processData(data, total);
      client.println(result);
      free(result);
      free(data);
    }
  }
  else if (_connected)
  {
    IpAddr ip;
    EEPROM.get(EEPROM_HOST_ADDR, ip);
    //Try once to reconnect
    if (!client.connect(ip.addr, 8081))
    {
      _connected = false;
      WiFi.listen();
    }
  }


}
