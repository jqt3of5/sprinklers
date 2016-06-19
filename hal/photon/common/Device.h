#ifndef DEVICE_H
#define DEVICE_H
class Device
{
public:

  Device() { ConfigPins(); }
  
  virtual void ConfigPins() = 0;
  virtual const char * GetDeviceType() = 0;

  const char * GetDeviceId()
  {
    char * tempId = spark_deviceID().c_str();
    char * id = new char[strlen(tempId) + 1];
    strcpy(id, tempId);
    return id;
  }

  char * Serialize()
  {
    const char * devTag = "deviceId";
    const char * deviceId = GetDeviceId();
    const char * typeTag = "type";
    const char * type = GetDeviceType();
    int len = strlen(devTag) + strlen(deviceId)
            + strlen(typeTag) + strlen(type);

    char * result = new char[2*len];
    sprintf(result, "{\"%s\":\"%s\", \"%s\":\"%s\"}", "type", type, "deviceId", deviceId);


    return result;
  }
  protected:

};
#endif
