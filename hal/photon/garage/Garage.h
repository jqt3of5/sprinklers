#include "IDevice.h"

class Garage : public IDevice
{
public:
  Garage();

  void ConfigPins();
  char * ProcessData(char * data, int count);

  char * GetDeviceType();

private:
  bool _lightOverride;
  int _lightTimeoutSeconds;
  Timer * _motionTimer;
  Timer * _overrideTimer;

  void MotionSensed();
  void LightTimedOut();
  void LightOverrideTimedOut();
  void ToggleLight();
  void ToggleGarage();
};
