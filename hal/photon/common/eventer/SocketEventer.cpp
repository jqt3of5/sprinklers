#include <Particle.h>
#include "SocketEventer.h"

SocketEventer::SocketEventer(char * url, int port, const char * deviceId)
{
  _url = new char[strlen(url) +1]();
  strcpy(_url, url);

  _deviceId = new char[strlen(deviceId) + 1]();
  strcpy(_deviceId, deviceId);

  _port = port;
}

void SocketEventer::eventNotification(char * event, char * data)
{
  TCPClient client;
  byte addr[4] = {52,34,40,4};
  if (client.connect(addr, _port))
  {
    /*char jsonObj[100] = {0};
    sprintf(jsonObj, "{\"deviceId\":\"%s\", \"%s\":\"%s\"}",_deviceId, event, data);
    client.write((uint8_t*)jsonObj, strlen(jsonObj));
    client.stop();*/
  }
}
