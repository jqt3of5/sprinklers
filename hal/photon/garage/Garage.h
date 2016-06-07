#include "IDevice.h"

class Garage : public IDevice
{
  void ConfigPins();
  char * ProcessData(char * data, int count);

  char * GetDeviceType();
};
