#ifndef DEVICE_H
#define DEVICE_H
#include <Particle.h>

class Device
{
public:

  Device() { ConfigPins(); }

  virtual void ConfigPins() {};
  virtual const char * GetDeviceType() = 0;

  char * Serialize()
  {
    const char * devTag = "deviceId";
    const char * deviceId = "asdf";//spark_deviceID().c_str();
    const char * typeTag = "type";
    const char * type = GetDeviceType();
    int len = strlen(devTag) + strlen(deviceId)
            + strlen(typeTag) + strlen(type);

    char * result = new char[2*len];
    sprintf(result, "{\"%s\":\"%s\", \"%s\":\"%s\"}", typeTag, type, devTag, deviceId);

    return result;
  }
  protected:

};
#endif
