
class Device
{
public:

  virtual void ConfigPins() = 0;
  virtual char * GetDeviceType() = 0;
  
  char * GetDeviceId()
  {
    return spark_deviceID().c_str();
  }
  
  char * Serialize()
  {
    char * devTag = "deviceId";
    char * deviceId = GetDeviceId();
    char * typeTag = "type";
    char * type = GetDeviceType();
    int len = strlen(devTag) + strlen(deviceId) 
            + strlen(typeTag) + strlen(type);
    
    char * result = new char[2*len];
    sprintf(result, "{'%s':'%s', '%s':'%s'}", "type", type, "deviceId", deviceId);
  
    return result; 
  }
  protected:
  
};
