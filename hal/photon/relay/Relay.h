#include "IDevice.h"

class Relay : public IDevice
{
  void ConfigPins();
  char * ProcessData(char * data, int count);

  char * GetDeviceType();
};
