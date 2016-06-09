#include "IDevice.h"

class Garage : public IDevice
{
  void ConfigPins();
  char * ProcessData(char * data, int count);

  char * GetDeviceType();
  
private:
  int _lightTimeoutSeconds;
  Timer * _timer;
  
  void MotionSensed();
  void LightTimedOut();
  
};
