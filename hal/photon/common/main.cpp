#pragma SPARK_NO_PREPROCESSOR
#include <Particle.h>
#include <softap_http.h>
#include "http.h"
//#include "IDevice.h"
#include "Garage.h"

const char * DEVICE_ID = "996903cc-f22a-4c03-94b1-fd050d942ef2";
const byte CURRENT_VERSION = 0;
const int EEPROM_VERSION = 0;
const int EEPROM_HOST_ADDR = 1;
//Really should have a class for this
//const int EEPROM_TOP = 5;

STARTUP(softap_set_application_page_handler(http_handler, nullptr));

IDevice * _device;
bool _connected = false;
TCPClient client;
void setup()
{
  _device = new Garage();
  _device->ConfigPins();

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
        client.printf("{\"type\":\"%s\", \"deviceId\":\"%s\"}",_device->GetDeviceType(), DEVICE_ID);
        _connected = true;
      }
  }
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
      char * result = _device->ProcessData(data, total);
      if (result != nullptr)
      {
        client.println(result);  
        free(result);
      }
      else
      {
        client.println("OK");
      }
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
