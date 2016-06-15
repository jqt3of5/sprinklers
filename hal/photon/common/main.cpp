#pragma SPARK_NO_PREPROCESSOR

SYSTEM_MODE(SEMI_AUTOMATIC);

#include <Particle.h>
#include <softap_http.h>
#include "http.h"
//#include "IDevice.h"
#include "Garage.h"

const byte CURRENT_VERSION = 0;
const int EEPROM_VERSION = 0;
const int EEPROM_HOST_ADDR = 1;
//Really should have a class for this
//const int EEPROM_TOP = 5;

STARTUP(softap_set_application_page_handler(http_handler, nullptr));

IDevice * _device;
bool _connected = false;
TCPClient _client;
void setup()
{
  _device = new Garage();
  _device->ConfigPins();

  byte version;
  EEPROM.get(EEPROM_VERSION, version);
  if (version == CURRENT_VERSION)
  {
    IpAddr ip;
    EEPROM.get(EEPROM_HOST_ADDR, ip);
    if (ip.addr[0] != 0xFF)
    {
      Particle.connect();
      _connected = true;
      return;
    }
  }
  else
  {
    //TODO: Migrate somehow?
    EEPROM.put(EEPROM_VERSION, CURRENT_VERSION);
  }
  //Error state. 
  WiFi.listen();
}

void loop()
{
  if (_client.connected())
  {
    readDataFromCloud();
    //TODO: Write data to cloud. Events/etc. 
  }
  //It is supposed to be connected to the cloud, but at this point isn't
  else if (_connected)
  {
    connectToCloud();
  }
}

void readDataFromCloud()
{
    int total = _client.available();
    if (total)
    {
      char * data = new char[total+1]();

      for (int i = 0; _client.available(); ++i)
      {
        data[i] = _client.read();
      }
      //Process Data
      char * result = _device->ProcessData(data, total);
      if (result != nullptr)
      {
        _client.println(result);  
        free(result);
      }
      else
      {
        _client.println("OK");
      }
      free(data);
    }
}
void connectToCloud()
{
    IpAddr ip;
    EEPROM.get(EEPROM_HOST_ADDR, ip);
    //Try once to reconnect
    if (_client.connect(ip.addr, 8081))
    {
       //Tell the cloud what type of device this is
        _client.printf(_device->Serialize());
    }
    else
    {
      _connected = false;
      WiFi.listen();
    }
}
