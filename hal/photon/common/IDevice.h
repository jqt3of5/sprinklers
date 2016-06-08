
class IDevice
{
public:
  virtual void ConfigPins() = 0;
  virtual char * ProcessData(char * data, int count) = 0;

  virtual char * GetDeviceType() = 0;
};